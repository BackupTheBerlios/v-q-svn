CREATE or replace FUNCTION mail.user_name
(mail.users.id_domain%TYPE,
mail.users.id_type%TYPE) RETURNS mail.users.login%TYPE AS '
declare
    _id_domain alias for $1;
    _uid alias for $2;
	name RECORD;
begin
	SELECT login INTO name FROM users 
		WHERE id_domain=_id_domain AND id_user=_uid LIMIT 1;
	IF NOT FOUND THEN
		RETURN -1;
	END IF;
	RETURN id.login;
end;
' LANGUAGE 'plpgsql';
