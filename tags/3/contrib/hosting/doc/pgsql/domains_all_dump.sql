create or replace function domains_all_dump (text,text) returns boolean as '
declare
    sep alias for $1;
    file alias for $2;
begin
    PERFORM dump2file3(''select dw.name,ip.ip from domains_all as dw,ip,domains as d where dw.domain_id=ip.domain AND d.id=dw.domain AND dw.name!=d.domain;'',sep,file);
    return ''t''::boolean;
end;
' language 'plpgsql';

create or replace function domains_all_dump () returns boolean as '
    SELECT domains_all_dump('' '',''/etc/pgsql/domains_all'');
' language 'sql';
