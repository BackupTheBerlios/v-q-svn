<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="news">
	<table>
		<xsl:for-each select="item">
			<tr>
				<td class="sec">
					<a>
						<xsl:attribute name="href">
							<xsl:text>http://sourceforge.net/forum/forum.php?forum_id=</xsl:text>
							<xsl:value-of select="@id"/>
						</xsl:attribute>
						<xsl:value-of select="@tit"/>
					</a>
				</td>
			</tr>
		</xsl:for-each>
	</table>
</xsl:template>

</xsl:stylesheet>
