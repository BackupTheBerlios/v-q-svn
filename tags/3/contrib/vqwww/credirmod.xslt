<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/redir">Przekierowanie</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/redir">
Umożliwia ustawienie przesyłania wiadomości na inne adresy. Przydatne
gdy np. jedziesz na urlop a chcesz aby ktoś zajmował się Twoimi sprawami.
</xsl:template>

<xsl:template match="vqwww/mod/redir">
<form method="post">
<table border="0">

<xsl:choose>
<xsl:when test="getcant">
<tr><td>Nie można pobrać aktualnej listy przekierowań.</td></tr>
</xsl:when>
<xsl:otherwise>

<tr><td colspan="2">
Uwaga: adresy nie są w żaden sposób walidowane, jeżeli wpiszesz
nieprawidłowy adres możesz doprowadzić do utraty części swojej
korespondencji. Przed ustawieniem przekierowania sprawdź czy adres działa.
</td></tr>

<!-- error messages -->
<xsl:for-each select="inv/rm">
<tr><td colspan="2">Nie można usunąć: <xsl:value-of select="text()"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/add">
<tr><td colspan="2">Nie można dodać: <xsl:value-of select="text()"/></td></tr>
</xsl:for-each>
<xsl:for-each select="inv/rep">
<tr><td colspan="2">Nie można zamienić: <xsl:value-of select="text()"/></td></tr>
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
<tr><td colspan="2"><input type="submit" value="Zmień"/></td></tr>
<input type="hidden" name="id" value="redir"/>
<input type="hidden" name="conf" value="1"/>

</xsl:otherwise>
</xsl:choose>

</table>
</form>
</xsl:template>

</xsl:stylesheet>
