CREATE TABLE "log" (
        "type" character(1) not null,
        "msg" text default '' not null,
        "ip" text default '' not null,
        "domain" text default '' not null,
        "login" text default '' not null,
        "time" timestamp with time zone DEFAULT now() not null,
        "msg1" text default '' not null
);

--
-- TOC Entry ID 48 (OID 288534)
--
-- Name: log Type: ACL Owner:
--

REVOKE ALL on "log" from PUBLIC;
GRANT ALL on "log" to "mail";
