CREATE or replace FUNCTION mail.user_rm
(mail.users.id_domain%TYPE,mail.users.id_user%TYPE) RETURNS VOID AS '
declare
    _id_domain alias for $1;
    _id_user alias for $2;
begin
	DELETE FROM users WHERE id_domain=_id_domain AND id_user=_id_user;
	NOTIFY user_rm;
	RETURN;
end;
' LANGUAGE 'plpgsql';
