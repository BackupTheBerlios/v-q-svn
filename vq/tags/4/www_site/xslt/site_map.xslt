<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="site_map">
	<xsl:apply-templates select="$menu" mode="site_map"/>
</xsl:template>

<!-- ###################################### -->
<xsl:template match="menu" mode="site_map">
	<ul>
		<li>
			<a>
				<xsl:attribute name="href">
					<xsl:value-of select="$base"/>
					<xsl:text>/</xsl:text>
				</xsl:attribute>
				<xsl:text>start</xsl:text>
			</a>
			
			<xsl:apply-templates select="sec" mode="site_map">
				<xsl:with-param name="path" select="$base"/>
			</xsl:apply-templates>
		</li>
	</ul>
</xsl:template>

<!-- ###################################### -->
<xsl:template match="sec" mode="site_map">
	<xsl:param name="path"/>
	<ul>
		<li>
			<a>	
				<xsl:attribute name="href">
					<xsl:choose>
						<xsl:when test="@url_sf">
							<xsl:text>http://</xsl:text>
							<xsl:value-of select="$proj_domain"/>
							<xsl:value-of select="@url_sf"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="$path"/>
							<xsl:text>/</xsl:text>
							<xsl:value-of select="@id"/>
							<xsl:value-of select="@ext"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:attribute>
				<xsl:value-of select="@tit"/>
			</a>

			<xsl:apply-templates select="sec" mode="site_map">
				<xsl:with-param name="path" select="concat($path,'/',@id)"/>
			</xsl:apply-templates>
		</li>
	</ul>
</xsl:template>

</xsl:stylesheet>
