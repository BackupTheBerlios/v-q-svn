create or replace function user_exist(text,text) returns int as '
declare
    dom alias for $1;
    user alias for $2;
    domrec RECORD;
begin
    SELECT INTO domrec 1 FROM pg_tables WHERE tablename=dom;
    IF NOT FOUND THEN
        RETURN 2;
    END IF;
    FOR domrec IN EXECUTE ''SELECT 1 FROM '' || quote_ident(dom)
            || '' WHERE login='' || quote_literal(user) LOOP
        RETURN 0;
    END LOOP;
    RETURN 1;
end;
' language 'plpgsql';
