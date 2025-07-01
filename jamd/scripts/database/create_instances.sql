create table instances
(
    id      serial
        primary key,
    name    text    not null,
    owner   integer not null,
    rlog    integer[],
    cmd     integer[],
    power   integer[],
    public  boolean default false,
    created integer not null,
    lastup  integer not null
);
