<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/useradd">Dodaj użytkownika</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/useradd">Tutaj możesz dodać
konto pocztowe, które będzie miało ograniczoną ważność po uiszczeniu
stosownej opłaty ważność konta zostanie przedłużona.</xsl:template>

<xsl:template match="vqwww/mod/useradd">
<form method="post">
<table border="0">
<xsl:if test="inv/login/@empty"><tr><td colspan="2">Podaj nazwę użytkownika.</td></tr></xsl:if>
<xsl:if test="inv/login/@dirty"><tr><td colspan="2">Nazwa użytkownika zawiera nieprawidłowe znaki.</td></tr></xsl:if>
<xsl:if test="inv/login/@tooshort"><tr><td colspan="2">Nazwa użytkownika jest za krótka.</td></tr></xsl:if>
<xsl:if test="inv/login/@toolong"><tr><td colspan="2">Nazwa użytkownika jest za długa.</td></tr></xsl:if>
<xsl:if test="inv/pass/@empty"><tr><td colspan="2">Podaj hasło.</td></tr></xsl:if>
<xsl:if test="inv/pass/@dirty"><tr><td colspan="2">Hasło zawiera nieprawidłowe znaki.</td></tr></xsl:if>
<xsl:if test="inv/pass/@tooshort"><tr><td colspan="2">Hasło jest za krótkie.</td></tr></xsl:if>
<xsl:if test="inv/pass/@toolong"><tr><td colspan="2">Hasło jest za długie.</td></tr></xsl:if>
<xsl:if test="inv/dontmatch"><tr><td colspan="2">Hasło i powtórka są różne.</td></tr></xsl:if>
<xsl:if test="inv/cant"><tr><td colspan="2">Nie można dodać użytkownika, jeżeli sytuacja powtarza się <a href="/admin.html">powiadom administratora</a>.</td></tr></xsl:if>
<xsl:if test="inv/done"><tr><td colspan="2">Dodano użytkownika.</td></tr></xsl:if>
<xsl:if test="inv/exists"><tr><td colspan="2">Użytkownik już istnieje.</td></tr></xsl:if>
<xsl:if test="inv/inv"><tr><td colspan="2">Nieprawidłowa nazwa użytkownika.</td></tr></xsl:if>

<tr><td>Nazwa użytkownika:</td><td>
<xsl:text disable-output-escaping="yes">&lt;input type="text" name="login" value="</xsl:text>
<xsl:value-of select="login/@val"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
</td></tr>
<tr><td>Hasło:</td><td><input type="password" name="pass"/></td></tr>
<tr><td>Powtórka hasła:</td><td><input type="password" name="pass1"/></td></tr>
<tr><td><input type="submit" value="Dodaj"/></td></tr>
<input type="hidden" name="id" value="useradd"/>
<input type="hidden" name="conf" value="1"/>
</table>
</form>
</xsl:template>

</xsl:stylesheet>
