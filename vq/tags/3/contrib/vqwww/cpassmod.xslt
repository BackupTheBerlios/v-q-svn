<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/pass">Zmień hasło</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/pass">
Pamiętaj aby zmieniać hasło co pewien czas, zbyt długie korzystanie
z tego samego hasła może ułatwić przejęcie Twojego konta przez 
nieupoważnione osoby.</xsl:template>

<xsl:template match="vqwww/mod/pass">
<xsl:choose>
<xsl:when test="inv/done">
Hasło zostało zmienione.
</xsl:when>
<xsl:otherwise>
<form method="post">
<table border="0">
<xsl:if test="inv/cpass/@empty"><tr><td colspan="2">Podaj aktualne hasło.</td></tr></xsl:if>
<xsl:if test="inv/npass/@empty"><tr><td colspan="2">Podaj nowe hasło.</td></tr></xsl:if>
<xsl:if test="inv/npass/@dirty"><tr><td colspan="2">Nowe hasło zawiera nieprawidłowe znaki.</td></tr></xsl:if>
<xsl:if test="inv/npass/@tooshort"><tr><td colspan="2">Nowe hasło jest za krótkie.</td></tr></xsl:if>
<xsl:if test="inv/npass/@toolong"><tr><td colspan="2">Nowe hasło jest za długie.</td></tr></xsl:if>
<xsl:if test="inv/dontmatch"><tr><td colspan="2">Nowe hasło i powtórka są różne.</td></tr></xsl:if>
<xsl:if test="inv/cant"><tr><td colspan="2">Nie można zmienić hasła, podałeś złe hasło lub nastąpił błąd systemu, jeżeli sytuacja powtarza się <a href="/admin.html">powiadom administratora</a>.</td></tr></xsl:if>
<tr><td>Aktualne hasło:</td><td><input type="password" name="cpass"/></td></tr>
<tr><td>Nowe hasło:</td><td><input type="password" name="npass"/></td></tr>
<tr><td>Powtórka hasła:</td><td><input type="password" name="rpass"/></td></tr>
<tr><td><input type="submit" value="Zmień"/></td></tr>
<input type="hidden" name="id" value="pass"/>
<input type="hidden" name="conf" value="1"/>
</table>
</form>
</xsl:otherwise>

</xsl:choose>
</xsl:template>

</xsl:stylesheet>
