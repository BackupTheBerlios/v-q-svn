CREATE OR REPLACE FUNCTION mail.eb_add (text,text) RETURNS INT4 AS '
declare
	_re_domain ALIAS FOR $1;
	_re_login ALIAS FOR $2;
	exists RECORD;
begin
	SELECT 1::integer INTO exists FROM emails_banned
		WHERE domain=_re_domain AND login=_re_login;
	IF FOUND THEN	
		RETURN -1;
	END IF;

	INSERT INTO emails_banned (domain,login) VALUES(_re_domain, _re_login);
	NOTIFY eb_add;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';
