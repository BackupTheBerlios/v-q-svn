CREATE or replace FUNCTION "dom_rm" (text) RETURNS VOID AS '
declare
    tab alias for $1;
begin
    EXECUTE ''drop table '' || quote_ident(tab) || '' CASCADE'';
    EXECUTE ''drop table '' || quote_ident(tab || ''_log'') || '' CASCADE'';
    EXECUTE ''drop table '' || quote_ident(tab || ''_dot'') || '' CASCADE'';
    EXECUTE ''DELETE FROM ip2domain WHERE domain='' || quote_literal(tab);
	EXECUTE ''DELETE FROM domains WHERE name ILIKE '' || quote_literal(tab);
	RETURN;
end;
' LANGUAGE 'plpgsql';
