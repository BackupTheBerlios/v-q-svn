CREATE OR REPLACE FUNCTION mail.dom_add (text) RETURNS mail.domains.id_domain%TYPE AS '
declare
    _domain ALIAS FOR $1;
	id RECORD;
	exists RECORD;
begin
	SELECT 1::integer INTO exists FROM domains WHERE domain=_domain;
	IF FOUND THEN	
		RETURN -1;
	END IF;

	SELECT NEXTVAL(''domains_id_domain_seq'') INTO id;
	
	INSERT INTO domains (id_domain,domain) VALUES(id.nextval, _domain);
	NOTIFY dom_add;
	RETURN id.nextval;
END;
' LANGUAGE 'plpgsql';
