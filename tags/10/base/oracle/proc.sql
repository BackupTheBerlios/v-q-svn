CREATE OR REPLACE FUNCTION dom_add (a_domain IN vq_domains.domain%TYPE) RETURN vq_domains.id_domain%TYPE IS
	CURSOR cur_exists IS SELECT -1 FROM vq_domains WHERE domain=a_domain;
	id vq_domains.id_domain%TYPE := -1;
BEGIN
	OPEN cur_exists;
	FETCH cur_exists INTO id;
	IF cur_exists%NOTFOUND = TRUE THEN
		SELECT vq_domains_id_domain_seq.NEXTVAL INTO id FROM DUAL;
		INSERT INTO vq_domains (id_domain,domain) VALUES(id, a_domain);
	END IF;
	CLOSE cur_exists;
	RETURN id;
END; 
/
show errors;

CREATE OR REPLACE FUNCTION dom_id(a_domain IN vq_domains.domain%TYPE) RETURN vq_domains.id_domain%TYPE IS
    id vq_domains.id_domain%TYPE := -1;
    CURSOR cur_domain IS SELECT id_domain FROM vq_domains WHERE domain=a_domain;
	CURSOR cur_alias IS SELECT id_domain FROM vq_domains_aliases WHERE
	alias=a_domain;
    CURSOR cur_ip IS SELECT id_domain FROM vq_domains_ips WHERE ip=a_domain OR '[' || ip || ']'=a_domain;
BEGIN
	OPEN cur_domain;
	FETCH cur_domain INTO id;
	IF cur_domain%NOTFOUND = TRUE THEN
		OPEN cur_alias;
		FETCH cur_alias INTO id;
		IF cur_alias%NOTFOUND = TRUE THEN
			OPEN cur_ip;
			FETCH cur_ip INTO id;
			CLOSE cur_ip;
		END IF;
		CLOSE cur_alias;
	END IF;
	CLOSE cur_domain;
	RETURN id;
END;
/
show errors;

CREATE or replace FUNCTION dom_rm (a_id_domain IN vq_domains.id_domain%TYPE) RETURN INTEGER IS
BEGIN
    DELETE FROM vq_domains WHERE id_domain = a_id_domain;
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE TRIGGER vq_domains_qt_user_trig BEFORE INSERT ON vq_domains FOR EACH ROW
DECLARE
    qt_files vq_domains.qt_user_files_def%TYPE;
    qt_bytes vq_domains.qt_user_bytes_def%TYPE;
    CURSOR cur_qt IS SELECT qt_user_files_def,qt_user_bytes_def FROM vq_domains WHERE domain='global';
BEGIN
    OPEN cur_qt;
    FETCH cur_qt INTO qt_files,qt_bytes;
    IF cur_qt%ROWCOUNT >= 1 THEN
        IF :NEW.qt_user_files_def = 0 OR :NEW.qt_user_files_def IS NULL THEN
            :NEW.qt_user_files_def := qt_files;
        END IF;

        IF :NEW.qt_user_bytes_def = 0 OR :NEW.qt_user_bytes_def IS NULL THEN
            :NEW.qt_user_bytes_def := qt_bytes;
        END IF;
    END IF;
    CLOSE cur_qt;
END;
/
show errors;

CREATE OR REPLACE FUNCTION da_add
(a_id_domain IN vq_domains_aliases.id_domain%TYPE, 
a_alias IN vq_domains_aliases.alias%TYPE) 
RETURN INTEGER IS
    CURSOR cur_exists IS SELECT -1 FROM vq_domains_aliases WHERE id_domain=a_id_domain AND alias=a_alias;
	ret INTEGER;
BEGIN
    OPEN cur_exists;
	FETCH cur_exists INTO ret;
    IF cur_exists%NOTFOUND = TRUE THEN
    	INSERT INTO vq_domains_aliases (id_domain,alias) 
			VALUES(a_id_domain, a_alias);
		ret := 0;
    END IF;
	CLOSE cur_exists;
    RETURN ret;
END;
/
show errors;

CREATE OR REPLACE FUNCTION da_rm
(a_alias IN vq_domains_aliases.alias%TYPE) 
RETURN INTEGER IS
BEGIN
    DELETE FROM vq_domains_aliases WHERE alias=a_alias;
	RETURN 0;
END;
/
show errors;

CREATE OR REPLACE FUNCTION dip_add
(a_id_domain IN vq_domains_ips.id_domain%TYPE, 
a_ip IN vq_domains_ips.ip%TYPE) 
RETURN INTEGER IS
    CURSOR cur_exists IS SELECT -1 FROM vq_domains_ips WHERE id_domain=a_id_domain AND ip=a_ip;
	ret INTEGER;
BEGIN
    OPEN cur_exists;
	FETCH cur_exists INTO ret;
    IF cur_exists%NOTFOUND = TRUE THEN
    	INSERT INTO vq_domains_ips (id_domain,ip) VALUES(a_id_domain, a_ip);
		ret := 0;
    END IF;
	CLOSE cur_exists;
    RETURN ret;
END;
/
show errors;

CREATE OR REPLACE FUNCTION dip_rm
(a_ip IN vq_domains_ips.ip%TYPE) 
RETURN INTEGER IS
BEGIN
    DELETE FROM vq_domains_ips WHERE ip=a_ip;
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE FUNCTION eb_add 
(a_re_domain IN vq_emails_banned.re_domain%TYPE, 
a_re_login IN vq_emails_banned.re_login%TYPE) RETURN INTEGER IS
    CURSOR cur_exists IS SELECT -1 FROM vq_emails_banned WHERE re_domain=a_re_domain AND re_login=a_re_login;
	ret INTEGER;
BEGIN
    OPEN cur_exists;
	FETCH cur_exists INTO ret;
    IF cur_exists%NOTFOUND = TRUE THEN
	    INSERT INTO vq_emails_banned (re_domain,re_login) VALUES(a_re_domain, a_re_login);
		ret := 0;
    END IF;
	CLOSE cur_exists;
    RETURN ret;
END;
/
show errors;

CREATE OR REPLACE FUNCTION eb_rm 
(a_re_domain IN vq_emails_banned.re_domain%TYPE, 
a_re_login IN vq_emails_banned.re_login%TYPE) 
RETURN INTEGER IS
BEGIN
    DELETE FROM vq_emails_banned WHERE re_domain = a_re_domain AND re_login=a_re_login;
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE FUNCTION qt_user_def_set
(a_id_domain IN vq_domains.id_domain%TYPE, 
a_qt_user_bytes_def IN vq_domains.qt_user_bytes_def%TYPE,
a_qt_user_files_def IN vq_domains.qt_user_files_def%TYPE) RETURN INTEGER IS
BEGIN
    UPDATE vq_domains SET qt_user_bytes_def=a_qt_user_bytes_def,qt_user_files_def=a_qt_user_files_def
        WHERE id_domain=a_id_domain;
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE FUNCTION qt_user_set
(a_id_domain IN vq_users.id_domain%TYPE, 
a_login IN vq_users.login%TYPE,
a_qt_bytes_max IN vq_users.qt_bytes_max%TYPE, 
a_qt_files_max IN vq_users.qt_files_max%TYPE) RETURN INTEGER IS
BEGIN
    UPDATE vq_users SET qt_bytes_max=a_qt_bytes_max,qt_files_max=a_qt_files_max
        WHERE id_domain=a_id_domain AND login=a_login;
    RETURN 0; 
END;
/
show errors;

CREATE OR REPLACE FUNCTION user_conf_add(a_id_domain IN vq_users_conf.id_domain%TYPE,
a_login IN vq_users_conf.login%TYPE,
a_ext IN vq_users_conf.ext%TYPE,
a_type IN vq_users_conf.type%TYPE,
a_val IN vq_users_conf.val%TYPE) RETURN INTEGER IS
    id vq_users_conf.ID_CONF%TYPE;
    CURSOR cur_domain IS SELECT 1 FROM vq_domains WHERE id_domain=a_id_domain;
    CURSOR cur_login IS SELECT 1 FROM vq_users WHERE id_domain=a_id_domain AND login=a_login;
BEGIN
    OPEN cur_domain;
	FETCH cur_domain INTO id;
    IF cur_domain%FOUND = TRUE THEN
		OPEN cur_login;
		FETCH cur_login INTO id;
		IF cur_login%FOUND = TRUE THEN
		    SELECT vq_users_conf_id_conf_seq.NEXTVAL INTO id FROM dual;
		    INSERT INTO vq_users_conf (ID_CONF,ID_DOMAIN,LOGIN,EXT,TYPE,VAL) 
        		VALUES(id,a_id_domain,a_login,a_ext,a_type,a_val);
		ELSE
			id := -2;
    	END IF;
		CLOSE cur_login;
	ELSE
		id := -1;
    END IF;
    CLOSE cur_domain;

    RETURN id;
END;
/
show errors;

CREATE OR REPLACE FUNCTION user_conf_rm (
a_id_conf IN vq_users_conf.id_conf%TYPE) RETURN INTEGER IS
BEGIN
    DELETE FROM vq_users_conf WHERE id_conf=a_id_conf;
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE FUNCTION user_conf_rm_by_type (a_id_domain IN vq_users_conf.id_domain%TYPE,
a_login IN vq_users_conf.login%TYPE, 
a_ext IN vq_users_conf.ext%TYPE,
a_type IN vq_users_conf.type%TYPE) RETURN INTEGER IS
BEGIN
	IF a_ext IS NULL THEN
    	DELETE FROM vq_users_conf WHERE id_domain = a_id_domain
    	    AND login=a_login AND ext IS NULL AND type=a_type;
	ELSE
	    DELETE FROM vq_users_conf WHERE id_domain = a_id_domain
    	    AND login=a_login AND ext=a_ext AND type=a_type;
	END IF;
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE FUNCTION user_conf_rep
(a_id_conf IN vq_users_conf.id_conf%TYPE, 
a_type IN vq_users_conf.type%TYPE, 
a_val IN vq_users_conf.val%TYPE) 
RETURN INTEGER IS
BEGIN
    UPDATE vq_users_conf SET type=a_type,val=a_val WHERE id_conf=a_id_conf;
    IF SQL%ROWCOUNT = 1 THEN
        RETURN 0;
    END IF;
    RETURN -1;
END;
/
show errors;

CREATE OR REPLACE FUNCTION user_pass
(a_id_domain IN vq_users.id_domain%TYPE, 
a_login IN vq_users.login%TYPE, 
a_pass IN vq_users.pass%TYPE) 
RETURN INTEGER IS
BEGIN
    UPDATE vq_users SET pass=a_pass WHERE id_domain=a_id_domain AND login=a_login;
    IF SQL%ROWCOUNT = 1 THEN
        RETURN 0;
    END IF;
    RETURN -1;
END;
/
show errors;

CREATE OR REPLACE FUNCTION user_add
(a_id_domain IN vq_users.id_domain%TYPE,
a_login IN vq_users.login%TYPE,
a_pass IN vq_users.pass%TYPE,
a_dir IN vq_users.dir%TYPE,
a_flags IN vq_users.flags%TYPE,
a_is_banned IN NUMBER) RETURN INTEGER IS
    CURSOR cur_is_banned IS SELECT 1 FROM vq_emails_banned WHERE 
      (SELECT UPPER(domain) FROM vq_domains WHERE id_domain=a_id_domain) LIKE UPPER(re_domain)
      AND UPPER(a_login) LIKE UPPER(re_login);
    CURSOR cur_domain IS SELECT 1 FROM vq_domains WHERE id_domain=a_id_domain;
    CURSOR cur_login IS SELECT 1 FROM vq_users WHERE id_domain=a_id_domain AND login=a_login;
	ret INTEGER;
BEGIN
	ret := 0;
    IF a_is_banned != 0 THEN
      OPEN cur_is_banned;
	  FETCH cur_is_banned INTO ret;
      IF cur_is_banned%ROWCOUNT = 1 THEN
        ret := -2;
      END IF;
      CLOSE cur_is_banned;

	  IF ret != 0 THEN
	  	RETURN ret;
	  END IF;
    END IF;

    OPEN cur_domain;
	FETCH cur_domain INTO ret;
    IF cur_domain%FOUND = TRUE THEN
	    OPEN cur_login;
		FETCH cur_login INTO ret;
	    IF cur_login%NOTFOUND = TRUE THEN
		    INSERT INTO vq_users (id_domain,login,pass,dir,flags) 
		        VALUES(a_id_domain,a_login,a_pass,a_dir,a_flags);
			ret := 0;
		ELSE
	        ret := -1;
	    END IF;
	    CLOSE cur_login;
	ELSE
	    ret := -3;
    END IF;
    CLOSE cur_domain;

    RETURN ret;
END;

/
show errors;

CREATE OR REPLACE FUNCTION user_rm
(a_id_domain IN vq_users.id_domain%TYPE, 
a_login IN vq_users.login%TYPE) RETURN INTEGER IS
BEGIN
    DELETE FROM vq_users WHERE id_domain=a_id_domain AND login=a_login;
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE TRIGGER vq_users_qt_user_trig BEFORE INSERT ON vq_users FOR EACH ROW
BEGIN
    IF :NEW.qt_files_max = 0 OR :NEW.qt_files_max IS NULL THEN
         SELECT qt_user_files_def INTO :NEW.qt_files_max FROM vq_domains WHERE id_domain=:NEW.id_domain;
        IF :NEW.qt_files_max IS NULL THEN
            :NEW.qt_files_max := 0;
        END IF;
    END IF;

    IF :NEW.qt_bytes_max = 0 OR :NEW.qt_bytes_max IS NULL THEN
        SELECT qt_user_bytes_def INTO :NEW.qt_bytes_max FROM vq_domains WHERE id_domain=:NEW.id_domain;
        IF :NEW.qt_bytes_max IS NULL THEN
            :NEW.qt_bytes_max := 0;
        END IF;
    END IF;
END;
/
show errors;

CREATE OR REPLACE FUNCTION log_write
(a_ip IN vq_log.ip%TYPE,
a_service IN vq_log.service%TYPE,
a_domain IN vq_log.domain%TYPE,
a_login IN vq_log.login%TYPE,
a_result IN vq_log.result%TYPE,
a_msg IN vq_log.msg%TYPE) RETURN INTEGER IS
	id vq_log.id_log%TYPE := -1;
BEGIN
	SELECT vq_log_id_log_seq.NEXTVAL INTO id FROM dual;
    INSERT INTO vq_log (id_log,ip,service,domain,login,result,msg) 
        VALUES(id,a_ip, a_service, a_domain, a_login, a_result, a_msg);
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE FUNCTION log_rm_all RETURN INTEGER IS
BEGIN
    DELETE FROM vq_log;
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE FUNCTION log_rm_by_dom(a_domain IN vq_log.domain%TYPE) RETURN INTEGER IS
BEGIN
    DELETE FROM vq_log WHERE domain=a_domain;
    RETURN 0;
END;
/
show errors;

CREATE OR REPLACE FUNCTION log_rm_by_user(
a_domain IN vq_log.domain%TYPE,a_login IN vq_log.login%TYPE) RETURN INTEGER IS
BEGIN
    DELETE FROM vq_log WHERE domain=a_domain AND login=a_login;
    RETURN 0;
END;
/
show errors;


