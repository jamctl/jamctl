create table users
(
    name          text not null,
    nickname      text not null,
    password      text,
    primary_email text,
    extra_emails  text[],
    "group"       text,
    constraint id
        primary key (name, nickname)
);

