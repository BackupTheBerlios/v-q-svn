CREATE or replace FUNCTION mail.dra_rm_by_dom (int4) RETURNS VOID AS '
declare
    _id_domain alias for $1;
begin
	DELETE FROM domains_re_aliases WHERE id_domain = _id_domain;
	NOTIFY dra_rm_by_dom;
	RETURN;
end;
' LANGUAGE 'plpgsql';
