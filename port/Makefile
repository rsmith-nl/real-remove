# New ports collection makefile for:	rrm
# Date created:				22 December 2008
# Whom:					rsmith@xs4all.nl
#
# $FreeBSD$

PORTNAME=	rrm
PORTVERSION=	1.0.1
CATEGORIES=	security
MASTER_SITES=	http://www.xs4all.nl/~rsmith/software/

MAINTAINER=	rsmith@xs4all.nl
COMMENT=	Overwrite and rename files before deleting them

PLIST_FILES=	bin/rrm

MAN1=		rrm.1
MANCOMPRESSED=	yes

.if defined(NOPORTDOCS)
MAKE_ENV=	NOPORTDOCS=yes
.else
PORTDOCS=	LICENSE README
.endif

post-patch:
	@${REINPLACE_CMD} -e \
		's|-pipe||; s|-fmerge-constants||; s|--fast-math||' \
		 ${WRKSRC}/Makefile

.include <bsd.port.mk>
