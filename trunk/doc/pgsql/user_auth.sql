CREATE or replace FUNCTION mail.user_auth
(mail.users.id_domain%TYPE,
mail.users.id_user%TYPE,
mail.users.pass%TYPE) RETURNS mail.users.flags%TYPE AS '
declare
	_id_domain ALIAS FOR $1;
	_id_user ALIAS FOR $2;
	_pass ALIAS FOR $3;
	ai RECORD;
begin
	SELECT INTO ai pass,flags FROM users 
		WHERE id_domain=_id_domain AND id_user=_id_user LIMIT 1;
	IF NOT FOUND THEN
		RETURN -1;
	END IF;
	IF ai.pass != _pass THEN
		RETURN -2;
	END IF;
	IF ai.flags < 0 THEN 
		RAISE EXCEPTION ''flags can not be lower than 0'';
	END IF;
	RETURN ai.flags;
end;
' LANGUAGE 'plpgsql';
