CREATE or replace FUNCTION "dom_rm" (text) RETURNS boolean AS '
declare
    dom alias for $1;
begin
    EXECUTE ''drop table '' || quote_ident(dom) || '' CASCADE'';
    EXECUTE ''drop table '' || quote_ident(dom || ''_log'') || '' CASCADE'';
    EXECUTE ''drop table '' || quote_ident(dom || ''_dot'') || '' CASCADE'';
    EXECUTE ''DELETE FROM ip2domain WHERE domain='' || quote_literal(dom);
    RETURN ''t''::boolean;
end;
' LANGUAGE 'plpgsql';
