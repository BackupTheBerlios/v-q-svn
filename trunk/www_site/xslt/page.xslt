<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" indent="yes" encoding="iso-8859-1"/>

<xsl:param name="site_name" select="'Virtual Qmail'"/>
<xsl:param name="proj_domain" select="'sourceforge.net/projects/v-q'"/>
<xsl:param name="sf_domain" select="'sourceforge.net'"/>
<xsl:param name="lists_domain" select="'lists.sourceforge.net'"/>
<xsl:param name="menu" select="document('menu.xml')"/>
<xsl:param name="main_width" select="'700'"/>
<xsl:param name="main_side_width" select="'170'"/>
<xsl:param name="base" select="'.'"/>

<xsl:include href="authors.xslt"/>
<xsl:include href="history.xslt"/>
<xsl:include href="upgrade.xslt"/>
<xsl:include href="site_map.xslt"/>
<xsl:include href="conf.xslt"/>
<xsl:include href="news.xslt"/>

<!-- ###################################### -->
<xsl:template match="menu">
	<xsl:param name="id"/>

	<table width="100%" cellspacing="0" cellpadding="0">
	<tr class="menu"><td>
		<xsl:for-each select="sec">
			<a>
				<xsl:attribute name="href">
					<xsl:choose>
						<xsl:when test="@url_sf">
							<xsl:text>http://</xsl:text>
							<xsl:value-of select="$sf_domain"/>
							<xsl:value-of select="@url_sf"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="$base"/>
							<xsl:text>/</xsl:text>
							<xsl:value-of select="@id"/>
							<xsl:value-of select="@ext"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:attribute>
				<xsl:value-of select="@tit"/>
			</a>
			<xsl:text> </xsl:text>
		</xsl:for-each>
	</td></tr>
	<tr class="sec"><td>
	<a>
		<xsl:attribute name="href">
			<xsl:value-of select="$base"/>
			<xsl:text>/</xsl:text>
		</xsl:attribute>
		<xsl:text>start</xsl:text>
	</a>

	<xsl:apply-templates select="sec[descendant-or-self::sec[@id=$id]]">
		<xsl:with-param name="path">
			<xsl:value-of select="$base"/>
		</xsl:with-param>
		<xsl:with-param name="id" select="$id"/>
	</xsl:apply-templates>
	</td></tr>
	</table>
</xsl:template>

<!-- ###################################### -->
<xsl:template match="menu//sec">
	<xsl:param name="path"/>
	<xsl:param name="id"/>

	<xsl:choose>
		<xsl:when test="sec[descendant-or-self::sec[@id=$id]]">
			<xsl:text> :: </xsl:text>
			<a>
				<xsl:attribute name="href">
					<xsl:value-of select="concat($path, '/', @id)"/>
					<xsl:value-of select="@ext"/>
				</xsl:attribute>
				<xsl:value-of select="@tit"/>
			</a>
			
			<xsl:apply-templates select="sec[descendant-or-self::sec[@id=$id]]">
				<xsl:with-param name="path" select="concat($path, '/', @id)"/>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:otherwise>
			<xsl:text> :: </xsl:text>
			<a>
				<xsl:attribute name="href">
					<xsl:value-of select="concat($path, '/', @id)"/>
					<xsl:value-of select="@ext"/>
				</xsl:attribute>
				<xsl:value-of select="@tit"/>
			</a>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- ###################################### -->
<xsl:template match="menu" mode="menu_side">
	<xsl:param name="id"/>

	<xsl:apply-templates select="sec[descendant-or-self::sec[@id=$id]]" mode="menu_side">
		<xsl:with-param name="path">
			<xsl:value-of select="$base"/>
		</xsl:with-param>
		<xsl:with-param name="id" select="$id"/>
	</xsl:apply-templates>

</xsl:template>

<!-- ###################################### -->
<xsl:template match="menu//sec" mode="menu_side">
	<xsl:param name="path"/>
	<xsl:param name="id"/>

	<xsl:choose>
		<xsl:when test="sec[descendant-or-self::sec[@id=$id]]">
			<xsl:apply-templates select="sec[descendant-or-self::sec[@id=$id]]" mode="menu_side">
				<xsl:with-param name="path" select="concat($path, '/', @id)"/>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:otherwise>
			<xsl:variable name="path1">
				<xsl:value-of select="concat($path, '/', @id)"/>
			</xsl:variable>
			
			<xsl:if test="descendant::sec or ancestor::sec/sec">
				<td valign="top" class="first">
				<table width="100%" class="first" cellspacing="0" cellpadding="0">
					<xsl:attribute name="width">
						<xsl:value-of select="$main_side_width"/>
					</xsl:attribute>
			
					<xsl:choose>
						<xsl:when test="descendant::sec">
							<xsl:for-each select="sec">
								<tr><td>
									<a>
										<xsl:attribute name="href">
											<xsl:choose>
												<xsl:when test="@url_sf">
													<xsl:text>http://</xsl:text>
													<xsl:value-of select="$sf_domain"/>
													<xsl:text>/</xsl:text>
													<xsl:value-of select="@url_sf"/>
												</xsl:when>
												<xsl:otherwise>
													<xsl:value-of select="concat($path1,'/', @id)"/>
													<xsl:value-of select="@ext"/>
												</xsl:otherwise>
											</xsl:choose>
										</xsl:attribute>
										<xsl:value-of select="@tit"/>
									</a>
								</td></tr>
							</xsl:for-each>
						</xsl:when>
						<xsl:otherwise>
							<xsl:for-each select="ancestor::sec/sec">
								<tr><td>
									<a>
										<xsl:attribute name="href">
											<xsl:choose>
												<xsl:when test="@url_sf">
													<xsl:text>http://</xsl:text>
													<xsl:value-of select="$sf_domain"/>
													<xsl:text>/</xsl:text>
													<xsl:value-of select="@url_sf"/>
												</xsl:when>
												<xsl:otherwise>
													<xsl:value-of select="concat($path,'/', @id)"/>
													<xsl:value-of select="@ext"/>
												</xsl:otherwise>
											</xsl:choose>
										</xsl:attribute>
										<xsl:value-of select="@tit"/>
									</a>
								</td></tr>
							</xsl:for-each>
						</xsl:otherwise>
					</xsl:choose>

				</table>
				</td>
			</xsl:if>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- ######################################### -->
<xsl:template match="/page">
	<xsl:text disable-output-escaping="yes">&lt;!DOCTYPE html public "-//W3C//DTD HTML 4.0 Transitional//EN"></xsl:text>
	<html>
		<head>
			<title><xsl:value-of select="$site_name"/></title>
			<meta name="author" content="Pawel Niewiadomski, new@foo-baz.com"/>
			<meta name="keywords" content="virtual, qmail, mta, domains, hosting, linux, openbsd, mail system"/>
			<link rel="stylesheet" type="text/css">
				<xsl:attribute name="href">
					<xsl:value-of select="$base"/>
					<xsl:text>/style.css</xsl:text>
				</xsl:attribute>
			</link>
		</head>
		<body bgcolor="white" color="black">
			
			<xsl:call-template name="inner"/>

		</body>
	</html>
</xsl:template>

<!-- ###################################### -->
<xsl:template name="inner">
	<table cellspacing="0" cellpadding="0" align="center">
		<xsl:attribute name="width">
			<xsl:value-of select="$main_width"/>
		</xsl:attribute>
		<tr>
			<td>
				<h2 class="sec">
					<xsl:value-of select="$site_name"/>
				</h2>
			</td>
		</tr>

		<tr>
			<td valign="top">
			 	<xsl:apply-templates select="$menu">
					<xsl:with-param name="id" select="@menu"/>
				</xsl:apply-templates>
			</td>
		</tr>
	</table>
	
	<table cellspacing="0" cellpadding="0" align="center"> 
		<xsl:attribute name="width">
			<xsl:value-of select="$main_width"/>
		</xsl:attribute>
	
		<tr>
			<xsl:apply-templates select="$menu" mode="menu_side">
				<xsl:with-param name="id" select="@menu"/>
			</xsl:apply-templates>
	
			<td width="100%">
				<xsl:choose>
					<xsl:when test="not(@spacing = 'off')">
						<table cellpadding="5" width="100%" height="200">
							<tr><td valign="top">
								<xsl:apply-templates/>
							</td></tr>
						</table>
					</xsl:when>
					<xsl:otherwise>
						<xsl:apply-templates/>
					</xsl:otherwise>
				</xsl:choose>
			</td>
		</tr>
		<tr>
			<td colspan="2" align="center" class="sec">
				<a href="http://new.foo-baz.com/">
					Copyrigth 2003, created by Pawel Niewiadomski
				</a>
			</td>
		</tr>
		<tr>
			<td colspan="2" align="center" class="first">
				<a href="http://sourceforge.net">
					<img src="http://sourceforge.net/sflogo.php?group_id=87916&amp;type=2" width="125" heigth="37" border="0" alt="SourceForge.net Logo"/>
				</a>
			</td>
		</tr>
	</table>
</xsl:template>

<xsl:template match="p">
	<xsl:copy>
	    <xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

<xsl:template match="table|ol|ul|br|a">
	<xsl:copy>
	    <xsl:copy-of select="node()|@*"/>
	</xsl:copy>
</xsl:template>

<xsl:template match="proj_site">
	<a>
		<xsl:attribute name="href">
			<xsl:text>http://</xsl:text>
			<xsl:value-of select="$proj_domain"/>
			<xsl:value-of select="@add"/>
		</xsl:attribute>
		<xsl:apply-templates/>
	</a>
</xsl:template>

<xsl:template match="page//sec">
	<h4 class="sec">
		<xsl:value-of select="@tit"/>
	</h4>
	<xsl:apply-templates/>
</xsl:template>

</xsl:stylesheet>

