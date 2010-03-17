-- partition functions

\c test_part0
drop function test_speed(text[], text[], text);
create or replace function test_speed(a text[], b text[], c text, out dbname text, out asize int4, out bsize int4)
as $$
    select current_database()::text as dbname, array_length($1, 1) as asize, array_length($2, 1) as bsize;
$$ language sql;

\c test_part1
drop function test_speed(text[], text[], text);
create or replace function test_speed(a text[], b text[], c text, out dbname text, out asize int4, out bsize int4)
as $$
    select current_database()::text as dbname, array_length($1, 1) as asize, array_length($2, 1) as bsize;
$$ language sql;

\c test_part2
drop function test_speed(text[], text[], text);
create or replace function test_speed(a text[], b text[], c text, out dbname text, out asize int4, out bsize int4)
as $$
    select current_database()::text as dbname, array_length($1, 1) as asize, array_length($2, 1) as bsize;
$$ language sql;

\c test_part3
drop function test_speed(text[], text[], text);
create or replace function test_speed(a text[], b text[], c text, out dbname text, out asize int4, out bsize int4)
as $$
    select current_database()::text as dbname, array_length($1, 1) as asize, array_length($2, 1) as bsize;
$$ language sql;


\c regression

-- \timing

drop function test_speed(text[], text[], text);
create or replace function test_speed(a text[], b text[], c text, out dbname text, out asize int4, out bsize int4)
returns setof record as $$
  cluster 'testcluster';
  split all;
  run on hashtext(a);
  --run on int4(random() * 64);
$$ language plproxy;

drop function test_speed_new(text[], text[], text);
create or replace function test_speed_new(a text[], b text[], c text, out dbname text, out asize int4, out bsize int4)
returns setof record as $$
  cluster 'testcluster';
  split all;
  newsplit;
  run on hashtext(a);
  --run on int4(random() * 64);
  select dbname, asize, bsize from test_speed(a,b,c);
$$ language plproxy;

\timing

\i sql/speed_test.sql

