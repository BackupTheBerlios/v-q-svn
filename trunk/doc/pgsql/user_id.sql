CREATE or replace FUNCTION mail.user_id
(mail.users.id_domain%TYPE,
mail.users.login%TYPE) RETURNS mail.users.id_user%TYPE AS '
declare
    _id_domain alias for $1;
    _login alias for $2;
	id RECORD;
begin
	SELECT id_user INTO id FROM users 
		WHERE id_domain=_id_domain AND login=_login LIMIT 1;
	IF NOT FOUND THEN
		RETURN -1;
	END IF;
	RETURN id.id_user;
end;
' LANGUAGE 'plpgsql';
