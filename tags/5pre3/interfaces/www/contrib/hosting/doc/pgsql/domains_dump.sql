create or replace function domains_dump (text,text) returns boolean as '
declare
    sep alias for $1;
    file alias for $2;
begin
    PERFORM dump2file3(''select d.domain,ip.ip from domains as d,ip where d.id=ip.domain'',sep,file);
    return ''t''::boolean;
end;
' language 'plpgsql';

create or replace function domains_dump () returns boolean as '
    SELECT domains_dump('' '',''/etc/pgsql/domains'');
' language 'sql';
