<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/redir">Przekierowanie</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/redir">
Umo�liwia ustawienie przesy�ania wiadomo�ci na inne adresy. Przydatne
gdy np. jedziesz na urlop a chcesz aby kto� zajmowa� si� Twoimi sprawami.
</xsl:template>

<xsl:template match="vqwww/mod/redir">
<form method="post">
<table border="0">

<xsl:choose>
<xsl:when test="getcant">
<tr><td>Nie mo�na pobra� aktualnej listy przekierowa�.</td></tr>
</xsl:when>
<xsl:otherwise>

<tr><td colspan="2">
Uwaga: adresy nie s� w �aden spos�b walidowane, je�eli wpiszesz
nieprawid�owy adres mo�esz doprowadzi� do utraty cz�ci swojej
korespondencji. Przed ustawieniem przekierowania sprawd� czy adres dzia�a.
</td></tr>

<!-- error messages -->
<xsl:for-each select="inv/rm">
<tr><td colspan="2">Nie mo�na usun��: <xsl:value-of select="text()"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/add">
<tr><td colspan="2">Nie mo�na doda�: <xsl:value-of select="text()"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/rep">
<tr><td colspan="2">Nie mo�na zamieni�: <xsl:value-of select="text()"/></td></tr>
</xsl:for-each>

<!-- input form -->
<xsl:for-each select="item">
<tr><td>Adres:</td><td>
<input>
<xsl:attribute name="name">
  <xsl:text>item</xsl:text><xsl:value-of select="position()"/>
</xsl:attribute>
<xsl:attribute name="value"><xsl:value-of select="@val"/></xsl:attribute>
</input>
<input type="hidden">
<xsl:attribute name="name">
  <xsl:text>itemid</xsl:text><xsl:value-of select="position()"/>
</xsl:attribute>
<xsl:attribute name="value">
  <xsl:value-of select="@id"/>
</xsl:attribute>
</input>
</td></tr>
</xsl:for-each>
<tr><td colspan="2"><input type="submit" value="Zmie�"/></td></tr>
<input type="hidden" name="id" value="redir"/>
<input type="hidden" name="conf" value="1"/>

</xsl:otherwise>
</xsl:choose>

</table>
</form>
</xsl:template>

</xsl:stylesheet>
