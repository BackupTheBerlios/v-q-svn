CREATE or replace FUNCTION mail.dom_rm (mail.domains.id_domain%TYPE) RETURNS VOID AS '
declare
    _id_domain alias for $1;
begin
	DELETE FROM domains WHERE id_domain = _id_domain;
	NOTIFY dom_rm;
	RETURN;
end;
' LANGUAGE 'plpgsql';
