CREATE FUNCTION mail.user_pass (int4,int4,text) RETURNS VOID AS '
declare
    _id_domain alias for $1;
    _id_user alias for $2;
    _pass alias for $3;
begin
	UPDATE users SET pass=_pass WHERE id_domain=_id_domain AND id_user=_id_user;
	RETURN;
end;
' LANGUAGE 'plpgsql';
