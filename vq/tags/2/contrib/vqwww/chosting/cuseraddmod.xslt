<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/useradd">Dodaj u�ytkownika</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/useradd">Tutaj mo�esz doda�
konto pocztowe, kt�re b�dzie mia�o ograniczon� wa�no�� po uiszczeniu
stosownej op�aty wa�no�� konta zostanie przed�u�ona.</xsl:template>

<xsl:template match="vqwww/mod/useradd">
<form method="post">
<table border="0">
<xsl:if test="inv/login/@empty"><tr><td colspan="2">Podaj nazw� u�ytkownika.</td></tr></xsl:if>
<xsl:if test="inv/login/@dirty"><tr><td colspan="2">Nazwa u�ytkownika zawiera nieprawid�owe znaki.</td></tr></xsl:if>
<xsl:if test="inv/login/@tooshort"><tr><td colspan="2">Nazwa u�ytkownika jest za kr�tka.</td></tr></xsl:if>
<xsl:if test="inv/login/@toolong"><tr><td colspan="2">Nazwa u�ytkownika jest za d�uga.</td></tr></xsl:if>
<xsl:if test="inv/pass/@empty"><tr><td colspan="2">Podaj has�o.</td></tr></xsl:if>
<xsl:if test="inv/pass/@dirty"><tr><td colspan="2">Has�o zawiera nieprawid�owe znaki.</td></tr></xsl:if>
<xsl:if test="inv/pass/@tooshort"><tr><td colspan="2">Has�o jest za kr�tkie.</td></tr></xsl:if>
<xsl:if test="inv/pass/@toolong"><tr><td colspan="2">Has�o jest za d�ugie.</td></tr></xsl:if>
<xsl:if test="inv/dontmatch"><tr><td colspan="2">Has�o i powt�rka s� r�ne.</td></tr></xsl:if>
<xsl:if test="inv/cant"><tr><td colspan="2">Nie mo�na doda� u�ytkownika, je�eli sytuacja powtarza si� <a href="/admin.html">powiadom administratora</a>.</td></tr></xsl:if>
<xsl:if test="inv/done"><tr><td colspan="2">Dodano u�ytkownika.</td></tr></xsl:if>
<xsl:if test="inv/exists"><tr><td colspan="2">U�ytkownik ju� istnieje.</td></tr></xsl:if>
<xsl:if test="inv/inv"><tr><td colspan="2">Nieprawid�owa nazwa u�ytkownika.</td></tr></xsl:if>

<tr><td>Nazwa u�ytkownika:</td><td>
<xsl:text disable-output-escaping="yes">&lt;input type="text" name="login" value="</xsl:text>
<xsl:value-of select="login/@val"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
</td></tr>
<tr><td>Has�o:</td><td><input type="password" name="pass"/></td></tr>
<tr><td>Powt�rka has�a:</td><td><input type="password" name="pass1"/></td></tr>
<tr><td><input type="submit" value="Dodaj"/></td></tr>
<input type="hidden" name="id" value="useradd"/>
<input type="hidden" name="conf" value="1"/>
</table>
</form>
</xsl:template>

</xsl:stylesheet>
