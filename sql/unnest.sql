
select * from plproxy.unnest(array[1,2,3], array[4,5,6]) as (a int4, b int4);

select * from plproxy.enumerate(array[1,2,3], array[4,5,6]) as (nr int4, a int4, b int4);

