CREATE OR REPLACE FUNCTION mail.dra_add (int4,text) RETURNS INT4 AS '
declare
	_id_domain ALIAS FOR $1;
	_re_alias ALIAS FOR $2;
	exists RECORD;
begin
	SELECT 1::integer INTO exists FROM domains_re_aliases
		WHERE id_domain=_id_domain AND re_alias=_re_alias;
	IF FOUND THEN	
		RETURN -1;
	END IF;

	INSERT INTO domains_re_aliases (id_domain,re_alias) 
		VALUES(_id_domain, _re_alias);
	NOTIFY dra_add;
	RETURN 0;
END;
' LANGUAGE 'plpgsql';
