<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="history">
	<xsl:if test="branch">
		<table width="100%">
			<xsl:for-each select="branch">
				<xsl:if test="version">
					<tr class="sec">
						<td>
							<xsl:choose>
								<xsl:when test="parent::history[@type='sum']">
									<xsl:text>Version: </xsl:text>
								</xsl:when>
								<xsl:otherwise>
									<xsl:text>Revision: </xsl:text>
								</xsl:otherwise>
							</xsl:choose>
							<xsl:value-of select="version"/>
						</td>

						<td>
							<xsl:text>Date: </xsl:text>
							<xsl:value-of select="date"/>
						</td>
					</tr>
					<xsl:if test="not(parent::history[@type='sum'])">
						<tr class="sec">
							<td colspan="2">
								<xsl:text>Author: </xsl:text>
								<xsl:value-of select="author"/>
							</td>
						</tr>
					</xsl:if>

					<tr>
						<td colspan="2">
							<xsl:apply-templates select="info"/>
						</td>
					</tr>

				</xsl:if>
			</xsl:for-each>
		</table>
	</xsl:if>
</xsl:template>

</xsl:stylesheet>
