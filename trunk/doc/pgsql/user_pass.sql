CREATE FUNCTION "user_pass" (text,text,text) RETURNS boolean AS '
declare
    user alias for $1;
    dom alias for $2;
    pass alias for $3;
begin
    EXECUTE ''update '' || quote_ident(dom) || '' set pass='' 
            || quote_literal(pass) || '' where login=''
            || quote_literal(user);
    RETURN ''t''::boolean;
end;
' LANGUAGE 'plpgsql';