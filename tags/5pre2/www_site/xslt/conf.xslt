<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="conf">
	<table>
		<xsl:for-each select="proj">
			<tr>
				<td class="first">
					Project: <xsl:value-of select="@name"/>
				</td>
			</tr>
			<xsl:for-each select="file">
				<tr>
					<td class="sec">
						File: <xsl:value-of select="@name"/>
					</td>
				</tr>
				<tr>
					<td>
						<xsl:apply-templates/>
					</td>
				</tr>
			</xsl:for-each>
		</xsl:for-each>
	</table>
</xsl:template>

</xsl:stylesheet>
