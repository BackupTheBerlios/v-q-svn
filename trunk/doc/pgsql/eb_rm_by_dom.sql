CREATE or replace FUNCTION mail.eb_rm (text,text) RETURNS VOID AS '
declare
    _re_domain alias for $1;
    _re_login alias for $2;
begin
	DELETE FROM emails_banned WHERE domain = _re_domain AND login=_re_login;
	NOTIFY eb_rm;
	RETURN;
end;
' LANGUAGE 'plpgsql';
