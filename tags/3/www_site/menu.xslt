<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="yes"/>

<xsl:template match="page">
<table class="n" width="170">
	<xsl:for-each select="files">
		<xsl:for-each select="file|log">
		<tr><td>
			<a>
			<xsl:attribute name="href">
				<xsl:value-of select="@name"/><xsl:text>.html</xsl:text>
			</xsl:attribute>
			<xsl:value-of select="@name"/>
			</a>
		</td></tr><tr><td>
			<xsl:value-of select="desc"/>
		</td></tr>
		</xsl:for-each>
		<xsl:for-each select="link">
		<tr><td>
			<a>
			<xsl:attribute name="href">
				<xsl:value-of select="@href"/>
			</xsl:attribute>
			<xsl:value-of select="@name"/>
			</a>
		</td></tr><tr><td>
			<xsl:value-of select="desc"/>
		</td></tr>
		</xsl:for-each>

	</xsl:for-each>
</table>
</xsl:template>

</xsl:stylesheet>

