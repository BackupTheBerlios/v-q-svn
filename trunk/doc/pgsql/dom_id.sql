CREATE or replace FUNCTION mail.dom_id (text) RETURNS int4 AS '
declare
    _domain alias for $1;
	id RECORD;
begin
	SELECT id_domain INTO id FROM domains WHERE domain=_domain LIMIT 1;
	IF NOT FOUND THEN
		RETURN -1;
	END IF;
	RETURN id.id_domain;
end;
' LANGUAGE 'plpgsql';
