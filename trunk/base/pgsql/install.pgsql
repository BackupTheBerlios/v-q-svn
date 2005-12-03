#!/usr/bin/perl

package main;
$Getopt::Std::STANDARD_HELP_VERSION = 1;
sub VERSION_MESSAGE {
	my $out = shift;
	print $out "$0 version 6\n";
}

sub HELP_MESSAGE {
	my $out = shift;
	print $out "usage: $0 [-c con_info] [-u user] [-p pass] [-s schema]\n";
	print $out "-c con_info\tPostgreSQL connection info\n";
	print $out "-u user\tconnect as this user\n";
	print $out "-p pass\tuse this password\n";
	print $out "-s schema\tcreate objects in specified schema\n";
}

package vqpgsql;

use DBI;
use Getopt::Std;

# Configuration

my %opts = ();
getopts('c:u:p:s:U', \%opts);

my $con_conf = $opts{'c'} ? $opts{'c'} : "dbname=mail host=/tmp";
my $con_user = $opts{'u'} ? $opts{'u'} : "mail";
my $con_pass = $opts{'p'} ? $opts{'p'} : "mail";
my $schema = $opts{'s'} ? $opts{'s'} : "mail";
my $upg = $opts{'U'};

# Prototypes
sub schema_crt;
sub schema_set;
sub qdie($);
sub version_get;
sub plpgsql;

# functions upgrading to version 9
sub vcur_funcs;
sub vcur_tables;

# functions upgrading
sub v8_9_funcs;
sub v7_8_funcs;
sub v6_7_funcs;

# generic vq_info update
sub upd_vq_info($);
sub ins_vq_info($);

############
my $con = DBI->connect("dbi:Pg:$con_conf", $con_user, $con_pass);
if( $con == undef ) {
	die("Can't connect");
}
if( $con->err != PGRES_CONNECTION_OK ) {
	die("Can't connect: ".$con->errstr);
}

if($schema ne "") {
	schema_crt();
	schema_set();
}

plpgsql();

my $ver = version_get();
my $cur_ver = 9;
print "Database version is: $ver\n";

if( $ver == 0 ) {
	print "Upgrading to $cur_ver...\n";
	ins_vq_info($cur_ver);
	vcur_tables();
	vcur_funcs();
} elsif($upg) {
	if( $ver >=  6 ) {
		if ($ver == 6) {
			v6_7_funcs();
			upd_vq_info(7);
			$ver = 7;
		}
		if ($ver == 7) {
			v7_8_funcs();
			upd_vq_info(8);
			$ver = 8;
		}
		if ($ver == 8) {
			v8_9_funcs();
			upd_vq_info(9);
			$ver = 9;
		}
	} else {
		die( "Don't know how to upgrade!" );
	}
} else {
	die("Database exists, upgrade must be forced with -U option!");
}

# Functions

##
#
sub schema_crt {
	my $qr = "SELECT '1' FROM pg_catalog.pg_namespace WHERE nspname='$schema'";
	my $sth = $con->prepare($qr);
	$sth->execute;
	my $res = $sth->fetchall_arrayref;
	if( $sth->err != PGRES_TUPLES_OK ) {
		qdie($qr);
	}
	if( @{$res}."" == 1 && ${$res}[0][0] eq "1" ) {
		return;
	}
	$qr = "CREATE SCHEMA $schema";
	$res = $con->do($qr);
	if( $con->err != PGRES_COMMAND_OK ) {
		qdie($qr);
	}
}

##
#
sub schema_set {
	my $qr = "SET search_path=$schema";
	my $res = $con->do($qr);
	if( $con->err != PGRES_COMMAND_OK ) {
		qdie($qr);
	}
}

##
#
sub qdie($) {
	die("Query: $_[0]: ".$con->errstr);
}

##
# Get v-q schema version
sub version_get {
	my $qr = "SELECT '1' FROM pg_tables WHERE schemaname='"
		.($schema eq "" ? "public" : $schema)."' AND tablename='vq_info'";
	my $sth = $con->prepare($qr);
	$sth->execute;
	my $res = $sth->fetchall_arrayref;
	if( $con->err != PGRES_TUPLES_OK ) {
		qdie($qr);
	}
	if( @{$res}."" == 1 && ${$res}[0][0] eq "1" ) {
		$qr = "SELECT value FROM vq_info WHERE key='version'";
		$sth = $con->prepare($qr);
		$sth->execute;
		$res = $sth->fetchall_arrayref;
		if( $con->err != PGRES_TUPLES_OK ) {
			qdie($qr);
		}
		return @{$res}."" >= 1 ? int(${$res}[0][0]) : 0;
	} else {
		return 0;
	}
}

##
# Create table with informations about database
sub ins_vq_info($) {
	my $ver = shift;
	my $qr = "CREATE TABLE vq_info "
		."(key TEXT NOT NULL CHECK (length(key)>1), value TEXT NOT NULL,".
		"PRIMARY KEY(key))";
	my $res = $con->do($qr);
	if( $con->err != PGRES_COMMAND_OK ) {
		qdie($qr);
	}
	$qr = "INSERT INTO vq_info (key,value) VALUES('version','".$ver."')";
	$res = $con->do($qr);
	if( $con->err != PGRES_COMMAND_OK || $res != 1 ) {
		qdie($qr);
	}
}

##
#
sub plpgsql {
	my $qr = "SELECT '1' FROM pg_catalog.pg_language WHERE lanname='plpgsql'";
	my $sth = $con->prepare($qr);
	$sth->execute;
	my $res = $sth->fetchall_arrayref;
	if( $con->err != PGRES_TUPLES_OK ) {
		qdie($qr);
	}
	if( @{$res}."" == 1 && ${$res}[0][0] eq "1" ) {
		return;
	}
	die("You must manually enable PL/PgSQL for selected database.\n");
}

##
#
sub vcur_tables {
	my @funcs = (
"CREATE TABLE vq_domains (id_domain serial,"
."domain text unique not null check(length(domain)>1),"
."qt_user_bytes_def int not null default 0,"
."qt_user_files_def int not null default 0,"
."PRIMARY KEY(id_domain))",

"CREATE TABLE vq_emails_banned (re_domain text not null,"
."re_login text not null,"
."PRIMARY KEY(re_domain, re_login))",

"CREATE TABLE vq_users (".
"id_domain int not null references vq_domains (id_domain) ON UPDATE CASCADE"
	." ON DELETE CASCADE,"
."login text not null check(login<>''),"
."pass text not null,"
."dir text not null default '',"
."flags int not null default 0 check(flags>=0),
qt_bytes_max int not null default 0 check(qt_bytes_max>=0),
qt_files_max int not null default 0 check(qt_files_max>=0),"
."PRIMARY KEY(id_domain, login))",

"CREATE TABLE vq_domains_aliases (
id_domain int not null references vq_domains (id_domain) on delete cascade on update cascade,
alias text not null,
PRIMARY KEY(alias))",

"CREATE INDEX vq_domains_aliases_id_domain_idx 
ON vq_domains_aliases (id_domain)",

"CREATE TABLE vq_domains_ips (
id_domain int not null references vq_domains (id_domain) on delete cascade on update cascade,
ip text not null,
PRIMARY KEY(ip))",

"CREATE INDEX vq_domains_ips_id_domain_idx 
ON vq_domains_ips (id_domain)",

"CREATE TABLE vq_users_conf (
id_conf SERIAL,
id_domain INT NOT NULL,
login TEXT NOT NULL,
ext TEXT NOT NULL,
type INT NOT NULL,
val TEXT NOT NULL,
PRIMARY KEY(id_conf),
FOREIGN KEY(id_domain, login) REFERENCES vq_users (id_domain, login) 
ON DELETE CASCADE ON UPDATE CASCADE 
)",

"CREATE TABLE vq_log (
id_log serial not null,
time timestamp not null default localtimestamp,
ip text not null default '',
service int not null default 0,
domain text not null default '',
login text not null default '',
result int not null default 0,
msg text not null default '',
primary key(id_log)
)",

"CREATE INDEX vq_log_time_idx ON vq_log (time)",

"CREATE INDEX vq_log_domain_idx ON vq_log (domain)",

"CREATE INDEX vq_log_domain_login_idx ON vq_log(domain, login)"
); # funcs

	for( my $i=0; $i < @funcs.""; ++$i ) {
		my $res = $con->do($funcs[$i]);
		if( $con->err != PGRES_COMMAND_OK ) {
			qdie($funcs[$i]);
		}
	}
} # vcur_tables

##
#
sub vcur_funcs {
	my @funcs = (
"CREATE FUNCTION dom_add(vq_domains.domain\%TYPE) RETURNS vq_domains.id_domain\%TYPE AS '
DECLARE
    _domain ALIAS FOR \$1;
	id RECORD;
BEGIN
	IF EXISTS (SELECT * FROM vq_domains WHERE domain=_domain) THEN
		RETURN -1;
	END IF;

	SELECT NEXTVAL(''vq_domains_id_domain_seq'') INTO id;
	
	INSERT INTO vq_domains (id_domain,domain) VALUES(id.nextval, _domain);
	NOTIFY dom_add;
	RETURN id.nextval;
END;
' LANGUAGE 'plpgsql';",

"CREATE FUNCTION dom_id(vq_domains.domain\%TYPE) RETURNS vq_domains.id_domain\%TYPE AS '
DECLARE
    _domain alias for \$1;
	id RECORD;
BEGIN
	SELECT id_domain INTO id FROM vq_domains WHERE domain=_domain LIMIT 1;
	IF NOT FOUND THEN
		SELECT id_domain INTO id FROM vq_domains_aliases WHERE alias=_domain LIMIT 1;
		IF NOT FOUND THEN
			SELECT id_domain INTO id FROM vq_domains_ips WHERE ip=_domain 
				OR ''['' || ip || '']''=_domain LIMIT 1;
			IF NOT FOUND THEN
				RETURN -1;
			END IF;
		END IF;
	END IF;
	RETURN id.id_domain;
END;
' LANGUAGE 'plpgsql';",

"CREATE VIEW vq_view_dom_name AS
SELECT domain,id_domain FROM vq_domains;",

"CREATE or replace FUNCTION dom_rm (vq_domains.id_domain\%TYPE) RETURNS INTEGER AS '
DECLARE
    _id_domain alias for \$1;
BEGIN
	DELETE FROM vq_domains WHERE id_domain = _id_domain;
	NOTIFY dom_rm;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';", 

"CREATE VIEW vq_view_dom_ls 
AS SELECT id_domain,domain FROM vq_domains ORDER BY DOMAIN",

"CREATE FUNCTION vq_domains_qt_user_trig() RETURNS TRIGGER AS '
DECLARE
	qt RECORD;
BEGIN
	SELECT INTO qt qt_user_files_def,qt_user_bytes_def FROM vq_domains 
		WHERE domain=''global'';

	IF FOUND THEN
		IF NEW.qt_user_files_def = 0 THEN
			NEW.qt_user_files_def := qt.qt_user_files_def;
		END IF;

		IF NEW.qt_user_bytes_def = 0 THEN
			NEW.qt_user_bytes_def := qt.qt_user_bytes_def;
		END IF;
	END IF;
	RETURN NEW;
END;
' LANGUAGE 'plpgsql';",

"CREATE TRIGGER vq_domains_qt_user_trig BEFORE INSERT ON vq_domains 
FOR EACH ROW EXECUTE PROCEDURE vq_domains_qt_user_trig ();",

"CREATE FUNCTION da_add
(vq_domains_aliases.id_domain\%TYPE, vq_domains_aliases.alias\%TYPE) 
RETURNS INT4 AS '
DECLARE
	_id_domain ALIAS FOR \$1;
	_alias ALIAS FOR \$2;
	exists RECORD;
BEGIN
	SELECT ''1'' INTO exists FROM vq_domains_aliases
		WHERE id_domain=_id_domain AND alias=_alias;
	IF FOUND THEN	
		RETURN -1;
	END IF;

	INSERT INTO vq_domains_aliases (id_domain,alias) 
		VALUES(_id_domain, _alias);
	NOTIFY da_add;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

"CREATE VIEW vq_view_da_ls AS
SELECT alias,id_domain FROM vq_domains_aliases ORDER BY alias",

"CREATE FUNCTION da_rm
(vq_domains_aliases.alias\%TYPE) 
RETURNS INTEGER AS '
DECLARE
    _alias alias for \$1;
BEGIN
	DELETE FROM vq_domains_aliases WHERE alias=_alias;
	NOTIFY da_rm;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

"CREATE FUNCTION dip_add
(vq_domains_ips.id_domain\%TYPE, vq_domains_ips.ip\%TYPE) 
RETURNS INT4 AS '
DECLARE
	_id_domain ALIAS FOR \$1;
	_alias ALIAS FOR \$2;
	exists RECORD;
BEGIN
	SELECT ''1'' INTO exists FROM vq_domains_ips
		WHERE id_domain=_id_domain AND ip=_alias;
	IF FOUND THEN	
		RETURN -1;
	END IF;

	INSERT INTO vq_domains_ips (id_domain,ip) 
		VALUES(_id_domain, _alias);
	NOTIFY DIP_ADD;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

"CREATE VIEW vq_view_dip_ls
AS SELECT ip,id_domain FROM vq_domains_ips ORDER BY ip",

"CREATE FUNCTION dip_rm
(vq_domains_ips.ip\%TYPE) 
RETURNS INTEGER AS '
DECLARE
    _alias alias for \$1;
BEGIN
	DELETE FROM vq_domains_ips WHERE ip=_alias;
	NOTIFY DIP_RM;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

"CREATE FUNCTION eb_add 
(vq_emails_banned.re_domain\%TYPE, vq_emails_banned.re_login\%TYPE) RETURNS INT4 AS '
DECLARE
	_re_domain ALIAS FOR \$1;
	_re_login ALIAS FOR \$2;
	exists RECORD;
BEGIN
	SELECT ''1'' INTO exists FROM vq_emails_banned
		WHERE re_domain=_re_domain AND re_login=_re_login;
	IF FOUND THEN	
		RETURN -1;
	END IF;

	INSERT INTO vq_emails_banned (re_domain,re_login) VALUES(_re_domain, _re_login);
	NOTIFY eb_add;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

"CREATE VIEW vq_view_eb_ls 
AS SELECT re_domain,re_login FROM vq_emails_banned ORDER BY re_domain,re_login",

"CREATE FUNCTION eb_rm 
(vq_emails_banned.re_domain\%TYPE, vq_emails_banned.re_login\%TYPE) 
RETURNS INTEGER AS '
DECLARE
    _re_domain alias for \$1;
    _re_login alias for \$2;
BEGIN
	DELETE FROM vq_emails_banned WHERE re_domain = _re_domain AND re_login=_re_login;
	NOTIFY eb_rm;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

"CREATE VIEW vq_view_QT_USER_DEF_GET AS
SELECT qt_user_bytes_def,qt_user_files_def,id_domain FROM vq_domains;",

"CREATE FUNCTION qt_user_def_set
(vq_domains.id_domain\%TYPE, 
vq_domains.qt_user_bytes_def\%TYPE,
vq_domains.qt_user_files_def\%TYPE) RETURNS INTEGER AS '
DECLARE
	_id_domain ALIAS FOR \$1;
	bytes_max ALIAS FOR \$2;
	files_max ALIAS FOR \$3;
BEGIN
	UPDATE vq_domains SET qt_user_bytes_def=bytes_max,qt_user_files_def=files_max
		WHERE id_domain=_id_domain;
	NOTIFY qt_user_def_set;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

"CREATE VIEW vq_view_qt_user_get AS
SELECT qt_bytes_max,qt_files_max,id_domain,login FROM vq_users;",

"CREATE FUNCTION qt_user_set
(vq_users.id_domain\%TYPE, vq_users.login\%TYPE,
vq_users.qt_bytes_max\%TYPE, vq_users.qt_files_max\%TYPE) RETURNS INTEGER AS '
DECLARE
	_id_domain ALIAS FOR \$1;
	_login ALIAS FOR \$2;
	bytes_max ALIAS FOR \$3;
	files_max ALIAS FOR \$4;
BEGIN
	UPDATE vq_users SET qt_bytes_max=bytes_max,qt_files_max=files_max
		WHERE id_domain=_id_domain AND login=_login;
	NOTIFY qt_user_set;
	RETURN 0; 
END;
' LANGUAGE 'plpgsql';",

"CREATE FUNCTION user_conf_add(vq_users_conf.id_domain\%TYPE,
	vq_users_conf.login\%TYPE,
	vq_users_conf.ext\%TYPE,vq_users_conf.type\%TYPE,vq_users_conf.val\%TYPE) 
	RETURNS int AS '
DECLARE
    _id_domain ALIAS FOR \$1;
    _login ALIAS FOR \$2;
    _ext ALIAS FOR \$3;
    _type ALIAS FOR \$4;
    _val ALIAS FOR \$5;
    id RECORD;
BEGIN
	IF NOT EXISTS (SELECT * FROM vq_domains WHERE id_domain=_id_domain) THEN
		RETURN -1;
	END IF;

	IF NOT EXISTS (SELECT * FROM vq_users WHERE id_domain=_id_domain 
			AND login=_login) THEN
		RETURN -2;
	END IF;

	SELECT NEXTVAL(''vq_users_conf_id_conf_seq'') INTO id;

	INSERT INTO vq_users_conf (ID_CONF,ID_DOMAIN,LOGIN,EXT,TYPE,VAL) 
		VALUES(id.nextval,_id_domain,_login,_ext,_type,_val);

	NOTIFY user_conf_add;
	RETURN id.nextval;
END;
' LANGUAGE 'plpgsql'",

"CREATE VIEW vq_view_user_conf_ls
AS SELECT id_conf,val,type,id_domain,login,ext FROM vq_users_conf",

"CREATE FUNCTION user_conf_rm (vq_users_conf.id_conf\%TYPE) RETURNS INTEGER AS '
DECLARE
    _id_conf alias for \$1;
BEGIN
	DELETE FROM vq_users_conf WHERE id_conf = _id_conf;
	NOTIFY user_conf_rm;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';", 

"CREATE FUNCTION user_conf_rm_by_type (vq_users_conf.id_domain\%TYPE,
	vq_users_conf.login\%TYPE, vq_users_conf.ext\%TYPE,
	vq_users_conf.type\%TYPE) RETURNS INTEGER AS '
DECLARE
    _id_domain alias for \$1;
    _login alias for \$2;
	_ext ALIAS FOR \$3;
    _type alias for \$4;
BEGIN
	DELETE FROM vq_users_conf WHERE id_domain = _id_domain
		AND login=_login AND ext=_ext AND type=_type;
	NOTIFY user_conf_rm;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';", 

"CREATE VIEW vq_view_user_conf_get AS
SELECT type,val,id_conf FROM vq_users_conf;",

"CREATE FUNCTION user_conf_rep
(vq_users_conf.id_conf\%TYPE, vq_users_conf.type\%TYPE, vq_users_conf.val\%TYPE) 
RETURNS INT AS '
DECLARE
    _id_conf alias for \$1;
    _type alias for \$2;
    _val alias for \$3;
	rows int;
BEGIN
	UPDATE vq_users_conf SET type=_type,val=_val WHERE id_conf=_id_conf;
	GET DIAGNOSTICS rows = ROW_COUNT;
	IF rows = 1 THEN
		NOTIFY user_conf_rep;
		RETURN 0;
	END IF;
	RETURN -1;
END;
' LANGUAGE 'plpgsql';",

"CREATE VIEW vq_view_user_conf_type_has AS
SELECT id_domain,login,ext,type FROM vq_users_conf;",

"CREATE VIEW vq_view_user_conf_type_cnt AS
SELECT COUNT(*) as count,id_domain,login,ext,type FROM vq_users_conf
GROUP BY id_domain,login,ext,type;",

"CREATE FUNCTION user_pass
(vq_users.id_domain\%TYPE, vq_users.login\%TYPE, vq_users.pass\%TYPE) 
RETURNS INT AS '
DECLARE
    _id_domain alias for \$1;
    _login alias for \$2;
    _pass alias for \$3;
	rows int;
BEGIN
	UPDATE vq_users SET pass=_pass WHERE id_domain=_id_domain AND login=_login;
	GET DIAGNOSTICS rows = ROW_COUNT;
	IF rows = 1 THEN
		NOTIFY user_pass;
		RETURN 0;
	END IF;
	RETURN -1;
END;
' LANGUAGE 'plpgsql';",


"CREATE VIEW vq_view_user_get AS
SELECT id_domain,login,pass,dir,flags FROM vq_users;",

"CREATE VIEW vq_view_user_ex AS
SELECT id_domain,login FROM vq_users;",

"CREATE FUNCTION user_rm
(vq_users.id_domain\%TYPE, vq_users.login\%TYPE) RETURNS INTEGER AS '
DECLARE
    _id_domain alias for \$1;
    _login alias for \$2;
BEGIN
	DELETE FROM vq_users WHERE id_domain=_id_domain AND login=_login;
	NOTIFY user_rm;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

"CREATE FUNCTION vq_users_qt_user_trig () RETURNS TRIGGER AS '
BEGIN
	IF NEW.qt_files_max = 0 THEN
		NEW.qt_files_max := (SELECT qt_user_files_def FROM vq_domains WHERE id_domain=NEW.id_domain);
		IF NEW.qt_files_max IS NULL THEN
			NEW.qt_files_max := 0;
		END IF;
	END IF;

	IF NEW.qt_bytes_max = 0 THEN
		NEW.qt_bytes_max := (SELECT qt_user_bytes_def FROM vq_domains WHERE id_domain=NEW.id_domain);
		IF NEW.qt_bytes_max IS NULL THEN
			NEW.qt_bytes_max := 0;
		END IF;
	END IF;

	RETURN NEW;
END;
' LANGUAGE 'plpgsql';",

"CREATE TRIGGER vq_users_qt_user_trig BEFORE INSERT ON vq_users 
FOR EACH ROW EXECUTE PROCEDURE vq_USERS_QT_USER_TRIG ();",

"CREATE or replace FUNCTION log_write
(vq_log.ip\%TYPE,
vq_log.service\%TYPE,
vq_log.domain\%TYPE,
vq_log.login\%TYPE,
vq_log.result\%TYPE,
vq_log.msg\%TYPE) RETURNS INTEGER AS '
DECLARE
    _ip alias for \$1;
    _service alias for \$2;
    _domain alias for \$3;
    _login alias for \$4;
    _result alias for \$5;
    _msg alias for \$6;
BEGIN
    INSERT INTO vq_log (ip,service,domain,login,result,msg) 
		VALUES(_ip, _service, _domain, _login, _result, _msg);

	NOTIFY log_write;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

"CREATE FUNCTION log_rm_all() RETURNS INTEGER AS '
BEGIN
	DELETE FROM vq_log;
	NOTIFY log_rm;
	RETURN 0;
END;' LANGUAGE 'plpgsql'",

"CREATE FUNCTION log_rm_by_dom(vq_log.domain\%TYPE) RETURNS INTEGER AS '
DECLARE
	_domain ALIAS FOR \$1;
BEGIN
	DELETE FROM vq_log WHERE domain=_domain;
	NOTIFY log_rm_by_dom;
	RETURN 0;
END;' LANGUAGE 'plpgsql'",

"CREATE FUNCTION log_rm_by_user(
vq_log.domain\%TYPE,vq_log.login\%TYPE) RETURNS INTEGER AS '
DECLARE
	_domain ALIAS FOR \$1;
	_login ALIAS FOR \$2;
BEGIN
	DELETE FROM vq_log WHERE domain=_domain AND login=_login;
	NOTIFY log_rm_by_user;
	RETURN 0;
END;' LANGUAGE 'plpgsql'",

"CREATE VIEW vq_view_log_count AS SELECT COUNT(*) FROM vq_log",

"CREATE VIEW vq_view_log_count_by_dom AS SELECT COUNT(*),domain FROM vq_log GROUP BY domain",

"CREATE VIEW vq_view_log_count_by_user AS "
."SELECT COUNT(*),domain,login FROM vq_log GROUP BY domain,login",

"CREATE VIEW vq_view_log_read AS "
."SELECT id_log,time,ip,service,domain,login,result,msg FROM vq_log"

);

	for( my $i=0; $i < @funcs.""; ++$i ) {
		my $res = $con->do($funcs[$i]);
		if( $con->err != PGRES_COMMAND_OK ) {
			qdie($funcs[$i]);
		}
	}

	v7_8_funcs;
	v8_9_funcs;
} # vcur_funcs

##
# v6_7_funcs 
sub v6_7_funcs {
	my @funcs = (
"CREATE or replace FUNCTION user_add
(vq_users.id_domain\%TYPE,
vq_users.login\%TYPE,
vq_users.pass\%TYPE,
vq_users.flags\%TYPE,
bool) RETURNS int4 AS '
DECLARE
    _id_domain alias for \$1;
    _login alias for \$2;
    _pass alias for \$3;
    _flags alias for \$4;
	is_banned alias for \$5;

	domain RECORD;
    ban RECORD;
BEGIN
    IF is_banned = ''t''::boolean THEN
      FOR ban IN SELECT 1 FROM vq_emails_banned WHERE 
	  		(SELECT domain FROM vq_domains WHERE id_domain=_id_domain) ILIKE re_domain
			AND _login ILIKE re_login LOOP
        RETURN -2;
      END LOOP;
    END IF;

	IF NOT EXISTS (SELECT * FROM vq_domains WHERE id_domain=_id_domain) THEN
		RETURN -3;
	END IF;

	IF EXISTS (SELECT * FROM vq_users 
			WHERE id_domain=_id_domain AND login=_login) THEN
		RETURN -1;
	END IF;
	
    INSERT INTO vq_users (id_domain,login,pass,flags) 
		VALUES(_id_domain,_login,_pass,_flags);

	NOTIFY user_add;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';"
);

	for( my $i=0; $i < @funcs.""; ++$i ) {
		my $res = $con->do($funcs[$i]);
		if( $con->err != PGRES_COMMAND_OK ) {
			qdie($funcs[$i]);
		}
	}
} # v6_7_funcs

##
# v7_8_funcs 
sub v7_8_funcs {
	my @funcs = (
"CREATE OR REPLACE VIEW vq_view_user_cnt_by_dom AS "
."SELECT id_domain,count(*) as count FROM vq_users GROUP BY id_domain",

"CREATE or replace FUNCTION user_add
(vq_users.id_domain\%TYPE,
vq_users.login\%TYPE,
vq_users.pass\%TYPE,
vq_users.dir\%TYPE,
vq_users.flags\%TYPE,
bool) RETURNS int4 AS '
DECLARE
    _id_domain alias for \$1;
    _login alias for \$2;
    _pass alias for \$3;
    _dir alias for \$4;
    _flags alias for \$5;
	is_banned alias for \$6;

    ban RECORD;
BEGIN
    IF is_banned = ''t''::boolean THEN
      FOR ban IN SELECT 1 FROM vq_emails_banned WHERE 
	  		(SELECT domain FROM vq_domains WHERE id_domain=_id_domain) ILIKE re_domain
			AND _login ILIKE re_login LOOP
        RETURN -2;
      END LOOP;
    END IF;

	IF NOT EXISTS (SELECT * FROM vq_domains WHERE id_domain=_id_domain) THEN
		RETURN -3;
	END IF;

	IF EXISTS (SELECT * FROM vq_users 
			WHERE id_domain=_id_domain AND login=_login) THEN
		RETURN -1;
	END IF;
	
    INSERT INTO vq_users (id_domain,login,pass,dir,flags) 
		VALUES(_id_domain,_login,_pass,_dir,_flags);

	NOTIFY user_add;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

);

	for( my $i=0; $i < @funcs.""; ++$i ) {
		my $res = $con->do($funcs[$i]);
		if( $con->err != PGRES_COMMAND_OK ) {
			qdie($funcs[$i]);
		}
	}
} # v7_8_funcs

##
# v8_9_funcs 
sub v8_9_funcs {
	my @funcs = (
"CREATE or replace FUNCTION user_rep
(vq_users.id_domain\%TYPE,
vq_users.login\%TYPE,
vq_users.pass\%TYPE,
vq_users.dir\%TYPE,
vq_users.flags\%TYPE,
bool, bool) RETURNS int4 AS '
DECLARE
    _id_domain alias for \$1;
    _login alias for \$2;
    _pass alias for \$3;
    _dir alias for \$4;
    _flags alias for \$5;
	pass_chg alias for \$6;
	dir_chg alias for \$7;

    ban RECORD;
BEGIN
	IF NOT EXISTS (SELECT * FROM vq_domains WHERE id_domain=_id_domain) THEN
		RETURN -1;
	END IF;

	IF NOT EXISTS (SELECT * FROM vq_users 
			WHERE id_domain=_id_domain AND login=_login) THEN
		RETURN -2;
	END IF;

    UPDATE vq_users SET flags=_flags
		WHERE id_domain=_id_domain AND login=_login;
	IF( pass_chg != false ) THEN
		 UPDATE vq_users SET pass=_pass
			WHERE id_domain=_id_domain AND login=_login;
	END IF;
	IF( dir_chg != false ) THEN
		 UPDATE vq_users SET dir=_dir
			WHERE id_domain=_id_domain AND login=_login;
	END IF;

	NOTIFY user_rep;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';",

);

	for( my $i=0; $i < @funcs.""; ++$i ) {
		my $res = $con->do($funcs[$i]);
		if( $con->err != PGRES_COMMAND_OK ) {
			qdie($funcs[$i]);
		}
	}
} # v8_9_funcs


##
# Create table with informations about database
sub upd_vq_info($) {
	my $ver = shift;
	$qr = "UPDATE vq_info SET value='".$ver."' WHERE key='version'";
	$res = $con->do($qr);
	if( $con->err != PGRES_COMMAND_OK || $res != 1 ) {
		qdie($qr);
	}
}
