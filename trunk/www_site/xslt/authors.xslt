<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="authors">
	<xsl:if test="item">
		<table>
			<xsl:for-each select="item">
				<tr>
					<xsl:if test="not(position() mod 2)">
						<xsl:attribute name="class">
							<xsl:text>sec</xsl:text>
						</xsl:attribute>
					</xsl:if>
					
					<td>
						<xsl:value-of select="@name"/>
					</td>
					<td>
						<xsl:if test="@email">
							<xsl:value-of select="substring-before(@email,'@')"/>
							<xsl:choose>
								<xsl:when test="position() mod 3 = 0">
									<xsl:text> __at__ </xsl:text>
								</xsl:when>
								<xsl:when test="position() mod 3 = 1">
									<xsl:text> (at) </xsl:text>
								</xsl:when>
								<xsl:when test="position() mod 3 = 2">
									<xsl:text> [_at_] </xsl:text>
								</xsl:when>
							</xsl:choose>
							<xsl:value-of select="substring-after(@email,'@')"/>
						</xsl:if>
					</td>
				</tr>
			</xsl:for-each>
		</table>
	</xsl:if>
</xsl:template>

</xsl:stylesheet>
