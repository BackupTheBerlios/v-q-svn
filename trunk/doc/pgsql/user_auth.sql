CREATE or replace FUNCTION mail.user_auth(text,text,text) RETURNS mail.users.flags%TYPE AS '
declare
	_id_domain ALIAS FOR $1;
	_id_user ALIAS FOR $2;
	_pass ALIAS FOR $3;
	_flags users.flags%TYPE;
begin
	SELECT INTO _flags flags FROM users 
		WHERE id_domain=_id_domain AND id_user=_id_user AND pass=_pass LIMIT 1;
	IF FOUND THEN
		RETURN _flags;
	END IF;
	RETURN NULL;
end;
' LANGUAGE 'plpgsql';
