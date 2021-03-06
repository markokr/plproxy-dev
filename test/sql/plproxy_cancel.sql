
-- test canceling

\c test_part0
create function rsleep(val int4) returns int4 as $$ begin perform pg_sleep(val); return 1; end; $$ language plpgsql;
\c test_part1
create function rsleep(val int4) returns int4 as $$ begin perform pg_sleep(val); return 1; end; $$ language plpgsql;
\c test_part2
create function rsleep(val int4) returns int4 as $$ begin perform pg_sleep(val); return 1; end; $$ language plpgsql;
\c test_part3
create function rsleep(val int4) returns int4 as $$ begin perform pg_sleep(val); return 1; end; $$ language plpgsql;
\c regression
create function rsleep(val int4, out res int4) returns setof int4 as $$
    cluster 'testcluster';
    run on all;
$$ language plproxy;

set statement_timeout = '1000';
select * from rsleep(10);

-- test if works later
select * from rsleep(0);

