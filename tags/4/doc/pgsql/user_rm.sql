CREATE or replace FUNCTION "user_rm" (text,text) RETURNS boolean AS '
declare
    user alias for $1;
    dom alias for $2;
begin
    EXECUTE ''delete from '' || quote_ident(dom || ''_log'' )
            || '' where login ILIKE '' || quote_literal(user);
    EXECUTE ''delete from '' || quote_ident(dom)
            || '' where login='' || quote_literal(user);
    RETURN ''t''::boolean;
end;
' LANGUAGE 'plpgsql';
