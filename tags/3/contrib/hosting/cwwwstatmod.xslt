<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/menu/mod/wwwstat">Statystyki WWW</xsl:template>

<xsl:template match="vqwww/mod/wwwstat">
<table border="0">

<xsl:choose>
<xsl:when test="getcant">
<tr><td>Nie można odczytać aktualnych ustawień.</td></tr>
</xsl:when>
<xsl:when test="noent">
<tr><td>Brak statystyk.</td></tr>
</xsl:when>
<xsl:otherwise>

<tr><td colspan="2">
Domeny: <xsl:for-each select="dom">
<xsl:text disable-output-escaping="yes">&lt;a href="#</xsl:text>
<xsl:value-of select="@name"/>
<xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
<xsl:value-of select="@name"/>
<xsl:text disable-output-escaping="yes">&lt;/A&gt;</xsl:text>
<xsl:text> </xsl:text>
</xsl:for-each>
</td></tr>

<!-- error messages -->

<!-- input form -->
<tr><th>Adres</th></tr>
<xsl:for-each select="dom">
    <xsl:text disable-output-escaping="yes">&lt;A NAME="</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    <xsl:for-each select="./item">
        <tr><td>
        <xsl:text disable-output-escaping="yes">&lt;A TARGET=_blank HREF="http://195.116.83.193/log/</xsl:text>
        <xsl:value-of select="ancestor::dom/@name"/>
        <xsl:text>/</xsl:text>
        <xsl:if test="@prefix!=''">
            <xsl:value-of select="@prefix"/><xsl:text>.</xsl:text>
        </xsl:if>
        <xsl:value-of select="ancestor::dom/@name"/>
        <xsl:text>/</xsl:text>
        <xsl:value-of select="ancestor::mod/wwwstat/@sid"/>
        <xsl:text disable-output-escaping="yes">/index.html"&gt;</xsl:text>
        <xsl:if test="@prefix!=''">
            <xsl:value-of select="@prefix"/><xsl:text>.</xsl:text>
        </xsl:if>
        <xsl:value-of select="ancestor::dom/@name"/>
        <xsl:text disable-output-escaping="yes">&lt;/A&gt;</xsl:text>
        </td></tr>
    </xsl:for-each>
</xsl:for-each>
</xsl:otherwise>
</xsl:choose>

</table>
</xsl:template>

</xsl:stylesheet>
