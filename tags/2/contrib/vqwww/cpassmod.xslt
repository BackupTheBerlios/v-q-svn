<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/pass">Zmie� has�o</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/pass">
Pami�taj aby zmienia� has�o co pewien czas, zbyt d�ugie korzystanie
z tego samego has�a mo�e u�atwi� przej�cie Twojego konta przez 
nieupowa�nione osoby.</xsl:template>

<xsl:template match="vqwww/mod/pass">
<xsl:choose>
<xsl:when test="inv/done">
Has�o zosta�o zmienione.
</xsl:when>
<xsl:otherwise>
<form method="post">
<table border="0">
<xsl:if test="inv/cpass/@empty"><tr><td colspan="2">Podaj aktualne has�o.</td></tr></xsl:if>
<xsl:if test="inv/npass/@empty"><tr><td colspan="2">Podaj nowe has�o.</td></tr></xsl:if>
<xsl:if test="inv/npass/@dirty"><tr><td colspan="2">Nowe has�o zawiera nieprawid�owe znaki.</td></tr></xsl:if>
<xsl:if test="inv/npass/@tooshort"><tr><td colspan="2">Nowe has�o jest za kr�tkie.</td></tr></xsl:if>
<xsl:if test="inv/npass/@toolong"><tr><td colspan="2">Nowe has�o jest za d�ugie.</td></tr></xsl:if>
<xsl:if test="inv/dontmatch"><tr><td colspan="2">Nowe has�o i powt�rka s� r�ne.</td></tr></xsl:if>
<xsl:if test="inv/cant"><tr><td colspan="2">Nie mo�na zmieni� has�a, poda�e� z�e has�o lub nast�pi� b��d systemu, je�eli sytuacja powtarza si� <a href="/admin.html">powiadom administratora</a>.</td></tr></xsl:if>
<tr><td>Aktualne has�o:</td><td><input type="password" name="cpass"/></td></tr>
<tr><td>Nowe has�o:</td><td><input type="password" name="npass"/></td></tr>
<tr><td>Powt�rka has�a:</td><td><input type="password" name="rpass"/></td></tr>
<tr><td><input type="submit" value="Zmie�"/></td></tr>
<input type="hidden" name="id" value="pass"/>
<input type="hidden" name="conf" value="1"/>
</table>
</form>
</xsl:otherwise>

</xsl:choose>
</xsl:template>

</xsl:stylesheet>
