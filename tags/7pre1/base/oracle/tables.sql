CREATE TABLE vq_info (
    key varchar2(1024) NOT NULL,
    value varchar2(1024)
);

CREATE TABLE vq_domains (
    id_domain INTEGER NOT NULL,
    domain varchar2(512) NOT NULL,
    qt_user_bytes_def integer DEFAULT 0 NOT NULL,
    qt_user_files_def integer DEFAULT 0 NOT NULL
);

CREATE TABLE vq_emails_banned (
    re_domain varchar2(512) NOT NULL,
    re_login varchar2(512) NOT NULL
);

CREATE TABLE vq_users (
    id_domain integer NOT NULL,
    login varchar2(512) NOT NULL,
    pass varchar2(512) NOT NULL,
    dir long,
    flags integer DEFAULT 0 NOT NULL,
    qt_bytes_max integer DEFAULT 0 NOT NULL,
    qt_files_max integer DEFAULT 0 NOT NULL,
    CONSTRAINT vq_users_flags CHECK ((flags >= 0)),
    CONSTRAINT vq_users_qtb_max CHECK ((qt_bytes_max >= 0)),
    CONSTRAINT vq_users_qtf_max CHECK ((qt_files_max >= 0))
);

CREATE TABLE vq_domains_aliases (
    id_domain integer NOT NULL,
    alias varchar2(512)
);

CREATE TABLE vq_domains_ips (
    id_domain integer NOT NULL,
    ip varchar2(100) NOT NULL
);

CREATE TABLE vq_users_conf (
    id_conf INTEGER NOT NULL,
    id_domain integer NOT NULL,
    login varchar2(512) NOT NULL,
    ext varchar2(512),
    type integer NOT NULL,
    val long
);

CREATE VIEW vq_view_dom_name AS
    SELECT vq_domains.domain, vq_domains.id_domain FROM vq_domains;

CREATE VIEW vq_view_dom_ls AS
    SELECT vq_domains.id_domain, vq_domains.domain FROM vq_domains;

CREATE VIEW vq_view_da_ls AS
    SELECT vq_domains_aliases.alias, vq_domains_aliases.id_domain FROM vq_domains_aliases;

CREATE VIEW vq_view_dip_ls AS
    SELECT vq_domains_ips.ip, vq_domains_ips.id_domain FROM vq_domains_ips;

CREATE VIEW vq_view_eb_ls AS
    SELECT vq_emails_banned.re_domain, vq_emails_banned.re_login FROM vq_emails_banned;

CREATE VIEW vq_view_qt_user_def_get AS
    SELECT vq_domains.qt_user_bytes_def, vq_domains.qt_user_files_def, vq_domains.id_domain FROM vq_domains;

CREATE VIEW vq_view_qt_user_get AS
    SELECT vq_users.qt_bytes_max, vq_users.qt_files_max, vq_users.id_domain, vq_users.login FROM vq_users;

CREATE VIEW vq_view_user_conf_ls AS
    SELECT vq_users_conf.id_conf, vq_users_conf.val, vq_users_conf.type, vq_users_conf.id_domain, vq_users_conf.login, vq_users_conf.ext FROM vq_users_conf;

CREATE VIEW vq_view_user_conf_get AS
    SELECT vq_users_conf.type, vq_users_conf.val, vq_users_conf.id_conf FROM vq_users_conf;

CREATE VIEW vq_view_user_conf_type_has AS
    SELECT vq_users_conf.id_domain, vq_users_conf.login, vq_users_conf.ext, vq_users_conf.type FROM vq_users_conf;

CREATE VIEW vq_view_user_conf_type_cnt AS
    SELECT count(*) AS count, vq_users_conf.id_domain, vq_users_conf.login, vq_users_conf.ext, vq_users_conf.type FROM vq_users_conf GROUP BY vq_users_conf.id_domain, vq_users_conf.login, vq_users_conf.ext, vq_users_conf.type;

CREATE VIEW vq_view_user_get AS
    SELECT vq_users.id_domain, vq_users.login, vq_users.pass, vq_users.dir, vq_users.flags FROM vq_users;

CREATE VIEW vq_view_user_ex AS
    SELECT vq_users.id_domain, vq_users.login FROM vq_users;

CREATE TABLE vq_log (
    id_log INTEGER NOT NULL,
    time timestamp WITH TIME ZONE DEFAULT current_timestamp NOT NULL,
    ip varchar2(100),
    service integer DEFAULT 0 NOT NULL,
    domain varchar2(512),
    login varchar2(512),
    result integer DEFAULT 0 NOT NULL,
    msg long
);

CREATE VIEW vq_view_log_count AS
    SELECT count(*) AS count FROM vq_log;

CREATE VIEW vq_view_log_count_by_dom AS
    SELECT count(*) AS count, vq_log.domain FROM vq_log GROUP BY vq_log.domain;

CREATE VIEW vq_view_log_count_by_user AS
    SELECT count(*) AS count, vq_log.domain, vq_log.login FROM vq_log GROUP BY vq_log.domain, vq_log.login;

CREATE VIEW vq_view_log_read AS
    SELECT vq_log.id_log, vq_log.time, vq_log.ip, vq_log.service, vq_log.domain, vq_log.login, vq_log.result, vq_log.msg FROM vq_log;    

CREATE INDEX vq_domains_aliases_id_dom_idx ON vq_domains_aliases (id_domain);

CREATE INDEX vq_domains_ips_id_domain_idx ON vq_domains_ips (id_domain);

CREATE INDEX vq_log_time_idx ON vq_log (time);

CREATE INDEX vq_log_domain_idx ON vq_log (domain);

CREATE INDEX vq_log_domain_login_idx ON vq_log (domain, login);

ALTER TABLE vq_info
    ADD CONSTRAINT vq_info_pkey PRIMARY KEY (key);

ALTER TABLE vq_domains
    ADD CONSTRAINT vq_domains_pkey PRIMARY KEY (id_domain);

ALTER TABLE vq_domains
    ADD CONSTRAINT vq_domains_domain_key UNIQUE (domain);

ALTER TABLE vq_emails_banned
    ADD CONSTRAINT vq_emails_banned_pkey PRIMARY KEY (re_domain, re_login);

ALTER TABLE vq_users
    ADD CONSTRAINT vq_users_pkey PRIMARY KEY (id_domain, login);

ALTER TABLE vq_domains_aliases
    ADD CONSTRAINT vq_domains_aliases_pkey PRIMARY KEY (alias);

ALTER TABLE vq_domains_ips
    ADD CONSTRAINT vq_domains_ips_pkey PRIMARY KEY (ip);

ALTER TABLE vq_users_conf
    ADD CONSTRAINT vq_users_conf_pkey PRIMARY KEY (id_conf);

ALTER TABLE vq_log
    ADD CONSTRAINT vq_log_pkey PRIMARY KEY (id_log);

ALTER TABLE vq_users
    ADD CONSTRAINT vq_users_id_domain_fkey FOREIGN KEY (id_domain) REFERENCES vq_domains(id_domain) ON DELETE CASCADE;

ALTER TABLE vq_domains_aliases
    ADD CONSTRAINT vq_domains_aliases_id_dom_fkey FOREIGN KEY (id_domain) REFERENCES vq_domains(id_domain) ON DELETE CASCADE;

ALTER TABLE vq_domains_ips
    ADD CONSTRAINT vq_domains_ips_id_dom_fkey FOREIGN KEY (id_domain) REFERENCES vq_domains(id_domain) ON DELETE CASCADE;

ALTER TABLE vq_users_conf
   ADD CONSTRAINT vq_users_conf_id_dom_log_fkey FOREIGN KEY (id_domain, login) REFERENCES vq_users(id_domain, login) ON DELETE CASCADE;

CREATE SEQUENCE vq_domains_id_domain_seq;

CREATE SEQUENCE vq_users_conf_id_conf_seq;

CREATE SEQUENCE vq_log_id_log_seq;
