<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="msgs" select="document(concat('freemail.',$LOCALE,'.xml'))/msgs"/>

<xsl:template match="vqwww/mod/conf/mod/ui">
	<xsl:value-of select="$msgs[@mod='freemail']/m[@n='mod']"/>
</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/ui">
	<xsl:value-of select="$msgs[@mod='freemail']/m[@n='mod/desc']"/>
</xsl:template>

<!-- MAIN HTML -->
<xsl:template match="vqwww/mod/ui">
<form method="post">
<table border="0">

<xsl:choose>
<xsl:when test="getcant">
<tr><td>Nie mo�na pobra� zapisanych informacji.</td></tr>
</xsl:when>

<xsl:otherwise>

<table border="0" width="500">
<!-- INV -->
<xsl:if test="inv/country/@sh">
    <tr><td colspan="2">Nazwa kraju jest za kr�tka.</td></tr>
</xsl:if>
<xsl:if test="inv/area/@inv">
    <tr><td colspan="2">Nieprawid�owe wojew�dztwo.</td></tr>
</xsl:if>
<xsl:if test="inv/area/@no">
    <tr><td colspan="2">Wybierz wojew�dztwo.</td></tr>
</xsl:if>
<xsl:if test="inv/city/@sh">
    <tr><td colspan="2">Nazwa miasta jest za kr�tka.</td></tr>
</xsl:if>
<xsl:if test="inv/birthday/@inv">
    <tr><td colspan="2">Data urodzenia jest nieprawid�owa.</td></tr>
</xsl:if>
<xsl:if test="inv/ints/@no">
    <tr><td colspan="2">Musisz poda� co najmniej jedno zainteresowanie.</td></tr>
</xsl:if>
<xsl:if test="chgcant">
    <tr><td colspan="2">Nie mo�na zmieni� informacji.</td></tr>
</xsl:if>

    <tr><td nowrap="">Kraj:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=country value="</xsl:text>
        <xsl:value-of select="country/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2">
    Minimalna d�ugo��: <xsl:value-of select="country/@minl"/>.
    </td></tr>
    <tr><td nowrap="">Wojew�dztwo:</td>
    <td><select name="area"><xsl:for-each select="areas/area">
        <xsl:text disable-output-escaping="yes">&lt;option value="</xsl:text>
        <xsl:value-of select="@id"/>
        <xsl:text disable-output-escaping="yes">" </xsl:text>
        <xsl:if test="@sel">
        <xsl:text disable-output-escaping="yes"> selected</xsl:text>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text disable-output-escaping="yes">&lt;/option&gt;</xsl:text>
    </xsl:for-each>
    </select></td></tr>
    <tr><td nowrap="">Miasto:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=city value="</xsl:text>
        <xsl:value-of select="city/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2">
    Minimalna d�ugo��: <xsl:value-of select="city/@minl"/>.
    </td></tr>
    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    <tr><td nowrap="">P�e�:</td>
    <td><select name="sex"><xsl:for-each select="sexes/sex">
        <xsl:text disable-output-escaping="yes">&lt;option value="</xsl:text>
        <xsl:value-of select="@id"/>
        <xsl:text disable-output-escaping="yes">" </xsl:text>
        <xsl:if test="@sel">
        <xsl:text disable-output-escaping="yes"> selected</xsl:text>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text disable-output-escaping="yes">&lt;/option&gt;</xsl:text>
    </xsl:for-each>
    </select></td></tr>
    <tr><td nowrap="">Data urodzenia:</td>
    <td><xsl:text disable-output-escaping="yes">&lt;input type=text name=birthday value="</xsl:text>
        <xsl:value-of select="birthday/@val"/>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    </td></tr>
    <tr><td colspan="2">
    Data w formacie YYYY-MM-DD (rok-miesi�c-dzie�).
    </td></tr>
    <tr><td nowrap="">Wykszta�cenie:</td>
    <td><select name="edu"><xsl:for-each select="edu/lev">
        <xsl:text disable-output-escaping="yes">&lt;option value="</xsl:text>
        <xsl:value-of select="@id"/>
        <xsl:text disable-output-escaping="yes">" </xsl:text>
        <xsl:if test="@sel">
        <xsl:text disable-output-escaping="yes"> selected</xsl:text>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text disable-output-escaping="yes">&lt;/option&gt;</xsl:text>
    </xsl:for-each>
    </select></td></tr>
    <tr><td nowrap="">Bran�a:</td>
    <td><select name="work"><xsl:for-each select="works/work">
        <xsl:text disable-output-escaping="yes">&lt;option value="</xsl:text>
        <xsl:value-of select="@id"/>
        <xsl:text disable-output-escaping="yes">" </xsl:text>
        <xsl:if test="@sel">
        <xsl:text disable-output-escaping="yes"> selected</xsl:text>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text disable-output-escaping="yes">&lt;/option&gt;</xsl:text>
    </xsl:for-each>
    </select></td></tr>
    <tr><td colspan="2">Zainteresowania:</td></tr>
    <xsl:for-each select="ints/int">
    <xsl:if test="position() mod 2 = 1">
    <xsl:text disable-output-escaping="yes">&lt;tr&gt;</xsl:text>
    </xsl:if>
    <td nowrap="">
        <xsl:text disable-output-escaping="yes">&lt;input type=checkbox name="int</xsl:text>
        <xsl:value-of select="position()"/>
        <xsl:text>" value="</xsl:text>
        <xsl:value-of select="@id"/>
        <xsl:text disable-output-escaping="yes">"</xsl:text>
        <xsl:if test="@sel">
            <xsl:text> checked </xsl:text>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        <xsl:value-of select="@name"/>
    </td>
    <xsl:if test="position() mod 2 = 0">
    <xsl:text disable-output-escaping="yes">&lt;/tr&gt;</xsl:text>
    </xsl:if>
    </xsl:for-each>


    <!-- -->
    <tr><td colspan="2"><hr/></td></tr>
    
    <tr><td nowrap="" colspan="2"><input type="submit" value="Zmie�"/></td></tr>
    </table>

    <input type="hidden" name="conf" value="1"/>
    <input type="hidden" name="id" value="ui"/>

</xsl:otherwise>
</xsl:choose>

</table>
</form>
</xsl:template>

</xsl:stylesheet>
