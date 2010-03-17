
select 'old', * from test_speed(
    (SELECT array_agg('user_' ||x) from generate_series(1,600) x),
    (SELECT array_agg('data_' ||x::text) from generate_series(1,600) x),
    'foo')
order by 1;

select 'new', * from test_speed_new(
    (SELECT array_agg('user_' ||x) from generate_series(1,600) x),
    (SELECT array_agg('data_' ||x::text) from generate_series(1,600) x),
    'foo')
order by 1;

select 'old', * from test_speed(
    (SELECT array_agg('user_' ||x) from generate_series(1,24000) x),
    (SELECT array_agg('data_' ||x::text) from generate_series(1,24000) x),
    'foo')
order by 1;

select 'new', * from test_speed_new(
    (SELECT array_agg('user_' ||x) from generate_series(1,24000) x),
    (SELECT array_agg('data_' ||x::text) from generate_series(1,24000) x),
    'foo')
order by 1;

