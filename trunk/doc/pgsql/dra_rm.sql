CREATE or replace FUNCTION mail.dra_rm (mail.domains_re_aliases.id_domain%TYPE,text) RETURNS VOID AS '
declare
    _id_domain alias for $1;
    _re_alias alias for $2;
begin
	DELETE FROM domains_re_aliases WHERE id_domain = _id_domain 
		AND re_alias=_re_alias;
	NOTIFY dra_rm;
	RETURN;
end;
' LANGUAGE 'plpgsql';
