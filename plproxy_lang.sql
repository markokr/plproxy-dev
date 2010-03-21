
create schema plproxy;
grant usage on schema plproxy to public;

-- handler function
CREATE FUNCTION plproxy.plproxy_call_handler()
RETURNS language_handler AS 'MODULE_PATHNAME' LANGUAGE C;

-- language
CREATE LANGUAGE plproxy HANDLER plproxy.plproxy_call_handler;

-- plproxy.unnest(...)
create function plproxy.unnest(anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_unnest' language C;
create function plproxy.unnest(anyarray, anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_unnest' language C;
create function plproxy.unnest(anyarray, anyarray, anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_unnest' language C;
create function plproxy.unnest(anyarray, anyarray, anyarray, anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_unnest' language C;

-- plproxy.enumerate(...)
create function plproxy.enumerate(anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_enumerate' language C;
create function plproxy.enumerate(anyarray, anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_enumerate' language C;
create function plproxy.enumerate(anyarray, anyarray, anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_enumerate' language C;
create function plproxy.enumerate(anyarray, anyarray, anyarray, anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_enumerate' language C;

create function plproxy.enumerate(int4[], anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_enumerate' language C;
create function plproxy.enumerate(text[], anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_enumerate' language C;
create function plproxy.enumerate(int4[], anyarray, anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_enumerate' language C;
create function plproxy.enumerate(text[], anyarray, anyarray)
returns setof record as 'MODULE_PATHNAME', 'multi_enumerate' language C;




