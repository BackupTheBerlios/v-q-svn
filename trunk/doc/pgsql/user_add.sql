CREATE or replace FUNCTION mail.user_add(int4,text,text,int4,bool) RETURNS int4 AS '
declare
    _id_domain alias for $1;
    _login alias for $2;
    _pass alias for $3;
    _flags alias for $4;
	is_banned alias for $5;

	domain RECORD;
	exists RECORD;
    user_id RECORD;
    ban RECORD;
begin
    IF is_banned = ''t''::boolean THEN
      FOR ban IN SELECT 1 FROM emails_banned WHERE 
	  		(SELECT domain FROM domains WHERE id_domain=_id_domain) ~* domain
			AND _login ~* login LOOP
        RETURN -2;
      END LOOP;
    END IF;

	
	FOR exists IN SELECT 1 FROM users 
			WHERE id_domain=_id_domain AND login=_login LOOP
		RETURN -1;
	END LOOP;
	
    SELECT NEXTVAL(''users_id_user_seq'') INTO user_id;
	
    INSERT INTO users (id_user,id_domain,login,pass,flags) 
		VALUES(user_id.nextval,_id_domain,_login,_pass,_flags);

	NOTIFY user_add;
	RETURN user_id.nextval;
end;
' LANGUAGE 'plpgsql';
