CREATE or replace FUNCTION mail.dra_ls_by_dom (mail.domains_re_aliases.id_domain%TYPE) RETURNS SETOF text AS '
declare
	_id_domain ALIAS FOR $1;
	rea RECORD;
begin
	FOR rea IN SELECT re_alias FROM domains_re_aliases WHERE id_domain=_id_domain LOOP
		RETURN NEXT rea.re_alias;
	END LOOP;
	RETURN;
end;
' LANGUAGE 'plpgsql';
