/*-------------------------------------------------------------------------
 *
 * arrayfuncs.c
 *	  Support functions for arrays.
 *
 * Portions Copyright (c) 1996-2010, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  $PostgreSQL$
 *
 *-------------------------------------------------------------------------
 */

#include <postgres.h>
#include <funcapi.h>
#include <catalog/pg_type.h>
#include <utils/array.h>
#include <utils/lsyscache.h>

struct ArrCtx {
	ArrayType  *arr;
	int			nextelem;
	int			numelems;
	char	   *elemdataptr;	/* this moves with nextelem */
	bits8	   *arraynullsptr;		/* this does not */
	int16		elmlen;
	bool		elmbyval;
	char		elmalign;
};

struct UnnestCtx {
	struct ArrCtx *arr_list;
	int todo;
	int num_arrays;
	int enumerate;

	// row
	TupleDesc desc;
	Datum values[FUNC_MAX_ARGS];
	char nulls[FUNC_MAX_ARGS];
};

/*
 * Utils from arrayfuncs.c
 */

static bool
array_get_isnull(const bits8 *nullbitmap, int offset)
{
	if (nullbitmap == NULL)
		return false;           /* assume not null */
	if (nullbitmap[offset / 8] & (1 << (offset % 8)))
		return false;           /* not null */
	return true; 
}

static Datum
ArrayCast(char *value, bool byval, int len)
{
	return fetch_att(value, byval, len);
}

/*
 * Load array info
 */
static void init_array(struct UnnestCtx *fctx, int i, ArrayType *arr)
{
	struct ArrCtx *actx = &fctx->arr_list[i];
	int idx = i + fctx->enumerate;

	actx->arr = arr;
	actx->nextelem = 0;

	if (!arr) {
		actx->numelems = 0;
		return;
	}

	actx->numelems = ArrayGetNItems(ARR_NDIM(arr), ARR_DIMS(arr));
	if (actx->numelems > fctx->todo)
		fctx->todo = actx->numelems;

	actx->elemdataptr = ARR_DATA_PTR(arr);
	actx->arraynullsptr = ARR_NULLBITMAP(arr);

	get_typlenbyvalalign(ARR_ELEMTYPE(arr),
						 &actx->elmlen,
						 &actx->elmbyval,
						 &actx->elmalign);
	TupleDescInitEntry(fctx->desc, idx + 1, NULL, ARR_ELEMTYPE(arr), 0, 0);
}

/*
 * Get value from array and add it to result tuple.
 */
static void add_value(struct UnnestCtx *fctx, int i)
{
	struct ArrCtx *actx = &fctx->arr_list[i];
	Datum elem;
	int offset;
	char *ptr;
	int idx = i + fctx->enumerate;

	fctx->nulls[idx] = 'n';
	fctx->values[idx] = 0;

	if (actx->nextelem >= actx->numelems)
		return;

	offset = actx->nextelem++;

	/*
	 * Check for NULL array element
	 */
	if (array_get_isnull(actx->arraynullsptr, offset))
		return;

	/*
	 * Get the element
	 */
	ptr = actx->elemdataptr;
	elem = ArrayCast(ptr, actx->elmbyval, actx->elmlen);

	/*
	 * Advance elemdataptr over it
	 */
	ptr = att_addlength_pointer(ptr, actx->elmlen, ptr);
	ptr = (char *) att_align_nominal(ptr, actx->elmalign);
	actx->elemdataptr = ptr;

	fctx->values[idx] = elem;
	fctx->nulls[idx] = ' ';
}

/*
 * UNNEST
 */

static Datum internal_unnest(FunctionCallInfo fcinfo, int enumerate)
{
	FuncCallContext *srfctx;
	struct UnnestCtx *fctx;
	MemoryContext oldcontext;
	int i;


	if (!fcinfo->flinfo || !fcinfo->flinfo->fn_retset)
		elog(ERROR, "please call as setof");

	/* stuff done only on the first call of the function */
	if (SRF_IS_FIRSTCALL())
	{
		/* create a function context for cross-call persistence */
		srfctx = SRF_FIRSTCALL_INIT();

		/*
		 * switch to memory context appropriate for multiple function calls
		 */
		oldcontext = MemoryContextSwitchTo(srfctx->multi_call_memory_ctx);

		fctx = palloc(sizeof(*fctx));
		fctx->enumerate = enumerate;
		fctx->arr_list = palloc(PG_NARGS() * sizeof(*fctx->arr_list));
		fctx->num_arrays = PG_NARGS();

		fctx->desc = CreateTemplateTupleDesc(fctx->num_arrays + fctx->enumerate, false);
		if (enumerate)
			TupleDescInitEntry(fctx->desc, 1, NULL, INT4OID, 0, 0);

		fctx->todo = -1;
		for (i = 0; i < fctx->num_arrays; i++) {
			bool isnull = PG_ARGISNULL(i);
			ArrayType *arr = isnull ? NULL : PG_GETARG_ARRAYTYPE_P(i);
			init_array(fctx, i, arr);
		}

		fctx->desc = BlessTupleDesc(fctx->desc);
		srfctx->user_fctx = fctx;
		MemoryContextSwitchTo(oldcontext);
	}

	/* stuff done on every call of the function */
	srfctx = SRF_PERCALL_SETUP();
	fctx = srfctx->user_fctx;

	if (fctx->todo > 0)
	{
		HeapTuple tup;
		for (i = 0; i < fctx->num_arrays; i++)
			add_value(fctx, i);
		fctx->todo--;
		if (fctx->enumerate) 
		{
			fctx->values[0] = Int32GetDatum(srfctx->call_cntr + 1);
			fctx->nulls[0] = ' ';
		}
		tup = heap_formtuple(fctx->desc, fctx->values, fctx->nulls);
		SRF_RETURN_NEXT(srfctx, HeapTupleGetDatum(tup));
	}
	else
	{
		SRF_RETURN_DONE(srfctx);
	}
}

/*
 * Public functions
 */

Datum multi_unnest(PG_FUNCTION_ARGS);
Datum multi_enumerate(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(multi_unnest);
PG_FUNCTION_INFO_V1(multi_enumerate);

Datum multi_unnest(PG_FUNCTION_ARGS)
{
	return internal_unnest(fcinfo, 0);
}

Datum multi_enumerate(PG_FUNCTION_ARGS)
{
	return internal_unnest(fcinfo, 1);
}

