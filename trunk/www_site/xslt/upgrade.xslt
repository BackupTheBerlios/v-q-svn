<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="upgrade">
	<table width="100%">
		<tr class="sec">
			<td>
				<xsl:text>From version: </xsl:text>
				<xsl:value-of select="@from"/>
				<xsl:text> to: </xsl:text>
				<xsl:value-of select="@to"/>
			</td>
		</tr>
		<tr>
			<td>
				<xsl:apply-templates/>
			</td>
		</tr>
	</table>
</xsl:template>

</xsl:stylesheet>
