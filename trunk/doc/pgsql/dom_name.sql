CREATE or replace FUNCTION mail.dom_name (text) RETURNS mail.domains.name%TYPE AS '
declare
    dom_id alias for $1;
	name RECORD;
begin
	SELECT domain INTO name FROM domains WHERE id_domain=dom_id LIMIT 1;
	IF NOT FOUND THEN
		RETURN -1;
	END IF;
	RETURN name.domain;
end;
' LANGUAGE 'plpgsql';
