create or replace function domains_prefixes_dump (text,text) returns boolean as '
declare
    sep alias for $1;
    file alias for $2;
begin
    PERFORM dump2file3(''select dp.prefix || \\\'.\\\' || dw.name,ip.ip from domains_prefixes as dp,domains_all as dw,ip where dp.domain_id=dw.id and dw.domain_id=ip.domain;'',sep,file);
    return ''t''::boolean;
end;
' language 'plpgsql';

create or replace function domains_prefixes_dump () returns boolean as '
    SELECT domains_prefixes_dump('' '',''/etc/pgsql/domains_prefixes'');
' language 'sql';
