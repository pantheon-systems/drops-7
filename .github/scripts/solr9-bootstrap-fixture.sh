#!/usr/bin/env bash
set -eou pipefail

# Bootstrap a D7 Solr 9 CI fixture site from scratch.
#
# Recreates the full dev environment that Solr 9 CUJ tests depend on.
# Run this if the fixture site is destroyed, corrupted, or needs to be
# rebuilt from zero.
#
# What it sets up:
#   1. Pantheon site on Drupal 7 upstream
#   2. Solr add-on enabled
#   3. Drupal install with admin credentials
#   4. 20 article nodes with real, searchable content
#   5. Verification
#
# Examples:
#   .github/scripts/solr9-bootstrap-fixture.sh -n search-api-pantheon-d7
#   .github/scripts/solr9-bootstrap-fixture.sh -n search-api-pantheon-d7 -o "CI Fixtures for Projects"
#
# Prerequisites:
#   - terminus authenticated (terminus auth:whoami)

show_help() {
    echo "Usage: $0 -n <site-name> [-o <org>]"
    echo "Options:"
    echo "  -n <arg>         Site name (e.g. search-api-pantheon-d7)"
    echo "  -o <arg>         Organization name or UUID (default: CI Fixtures for Projects)"
    echo "  -h               Show help"
    exit 1
}

main() {
    local SITE_NAME=""
    local ORG="CI Fixtures for Projects"

    while getopts "n:o:h" opt; do
        case $opt in
            n) SITE_NAME="$OPTARG" ;;
            o) ORG="$OPTARG" ;;
            h) show_help ;;
            *) show_help ;;
        esac
    done

    shift "$((OPTIND-1))"

    if [[ -z "$SITE_NAME" ]]; then
        echo "ERROR: -n <site-name> is required"
        show_help
    fi

    local SITE_NAME_LC
    SITE_NAME_LC=$(echo "$SITE_NAME" | tr '[:upper:]' '[:lower:]')
    local SITE_ENV="${SITE_NAME_LC}.dev"

    echo "=== Bootstrap D7 Solr 9 CI Fixture Site ==="
    echo "Site: ${SITE_NAME}"
    echo "Org: ${ORG}"
    echo ""

    # -----------------------------------------------------------------------
    # Step 1: Create site (exit if exists)
    # -----------------------------------------------------------------------
    if terminus site:info "$SITE_NAME_LC" &>/dev/null; then
        echo "ERROR: Site ${SITE_NAME} already exists. Exiting to avoid modifying an existing site."
        echo "Delete it first if you want to recreate: terminus site:delete ${SITE_NAME_LC}"
        exit 1
    fi

    echo "[1/5] Creating site..."
    terminus site:create "$SITE_NAME" "$SITE_NAME" "drupal7" --org="$ORG"
    echo "Waiting for site creation workflow..."
    terminus workflow:wait "$SITE_ENV"

    local SITE_ID
    SITE_ID=$(terminus site:info "$SITE_NAME_LC" --field=ID)

    # -----------------------------------------------------------------------
    # Step 2: Upgrade plan + enable Solr
    # -----------------------------------------------------------------------
    echo "[2/5] Configuring plan and Solr..."
    local CURRENT_PLAN
    CURRENT_PLAN=$(terminus site:info "$SITE_NAME_LC" --field=plan_name 2>/dev/null || echo "")
    if [[ "$CURRENT_PLAN" == *"Sandbox"* ]]; then
        echo "Upgrading to Performance Small..."
        terminus plan:set "$SITE_ID" "plan-performance_small-contract-annual-1"
    else
        echo "[skip] Already on plan: ${CURRENT_PLAN}"
    fi

    echo "Enabling Solr..."
    terminus solr:enable "$SITE_ID" 2>/dev/null || echo "[skip] Solr already enabled or enable failed"

    # -----------------------------------------------------------------------
    # Step 3: Install Drupal
    # -----------------------------------------------------------------------
    echo "[3/5] Installing Drupal..."
    if terminus drush "$SITE_ENV" -- status --field=bootstrap 2>/dev/null | grep -q "Successful"; then
        echo "[skip] Drupal already installed"
    else
        terminus drush "$SITE_ENV" -- site-install standard \
            --site-name="D7 Solr 9 CI" \
            --account-name=admin \
            --account-pass=admin \
            -y
    fi

    # -----------------------------------------------------------------------
    # Step 4: Create 20 article nodes with real content
    # -----------------------------------------------------------------------
    echo "[4/5] Creating article nodes..."

    terminus drush "$SITE_ENV" -- ev '
$articles = array(
  array("Getting Started with Apache Solr on Pantheon", "Apache Solr is a powerful open-source search platform built on Apache Lucene. Pantheon provides managed Solr instances for every environment, enabling fast full-text search across your Drupal content. This guide covers initial setup, schema posting, and indexing your first batch of content."),
  array("Understanding Drupal Content Types and Fields", "Drupal organizes content into types such as articles, pages, and custom bundles. Each content type can have fields for text, images, taxonomy references, and more. Properly structured content types improve both editorial workflow and search relevance."),
  array("WordPress to Drupal Migration Best Practices", "Migrating from WordPress to Drupal requires careful planning around content mapping, URL redirects, and user account migration. The Migrate module provides a framework for pulling content from external sources including WordPress databases and XML exports."),
  array("Performance Optimization for High Traffic Websites", "High traffic websites require careful attention to caching, database query optimization, and CDN configuration. Pantheon Global CDN provides edge caching with surrogate key purging, while Varnish handles full-page caching at the platform level."),
  array("Search Engine Optimization Techniques for Drupal", "Effective SEO in Drupal involves clean URL structures, proper meta tag configuration, XML sitemaps, and structured data markup. The Pathauto module generates search-friendly URLs automatically based on content patterns."),
  array("Configuring Multidev Environments on Pantheon", "Multidev environments allow teams to work on parallel feature branches without interfering with each other. Each multidev gets its own database, files, and server environment cloned from an existing environment."),
  array("Database Backup and Recovery Strategies", "Regular database backups are essential for disaster recovery. Pantheon provides automated daily backups and on-demand backup creation. Recovery involves downloading the backup archive and importing it into the target environment."),
  array("Custom Module Development in Drupal 7", "Building custom modules in Drupal 7 follows the hook system architecture. Modules implement hooks like hook_menu, hook_form_alter, and hook_node_view to extend core functionality. The module .info file declares dependencies and metadata."),
  array("Solr Schema Configuration and Field Mapping", "The Solr schema defines how content fields are indexed and searched. Field types control tokenization, stemming, and analysis chains. Proper field mapping ensures that search queries return relevant results with appropriate boosting."),
  array("Terminus Command Line Interface Reference", "Terminus is the command line interface for Pantheon. It provides commands for site management, environment operations, and workflow automation. Common commands include site creation, database operations, and deployment workflows."),
  array("Managing SSL Certificates for Custom Domains", "Custom domains on Pantheon require DNS configuration and SSL certificate provisioning. The platform provides free automated HTTPS through Let us Encrypt. Proper DNS setup includes A records, CNAME records, and CAA records for certificate authority authorization."),
  array("Cron Job Scheduling and Automation in Drupal", "Drupal cron runs periodic maintenance tasks including content indexing, cache clearing, and queue processing. On Pantheon, cron runs automatically every hour. Custom cron intervals can be configured using the Elysia Cron module or hook_cron implementations."),
  array("User Authentication and Role Management", "Drupal provides a robust permission system with roles and granular access controls. Users can be assigned multiple roles, each granting specific permissions. SAML and OAuth integrations enable single sign-on with external identity providers."),
  array("Responsive Theme Development with Drupal 7", "Building responsive themes in Drupal 7 requires media queries, fluid grids, and flexible images. The theme layer uses template files, preprocess functions, and the render API. Mobile-first design ensures optimal experience across devices."),
  array("Version Control Workflows for Drupal Teams", "Git-based workflows on Pantheon support feature branching, code review, and automated deployments. Teams typically use a trunk-based development model with short-lived feature branches. Pull requests enable peer review before merging to the main branch."),
  array("Debugging PHP Errors and Watchdog Logs", "Drupal watchdog logs capture errors, warnings, and informational messages from modules and core. The dblog module stores entries in the database accessible at admin reports dblog. Common PHP errors include undefined function calls, memory limits, and deprecated function usage."),
  array("Taxonomy and Vocabulary Configuration Guide", "Taxonomy provides hierarchical classification for Drupal content. Vocabularies group related terms, and term references connect content to classification schemes. Faceted search leverages taxonomy to filter results by category, tag, or other classification dimensions."),
  array("Integrating Third Party APIs with Drupal", "Drupal modules can consume external APIs using drupal_http_request or the Guzzle HTTP client. API keys and credentials should be stored securely using the Key module or environment variables. Rate limiting and caching prevent excessive API calls."),
  array("Content Moderation and Editorial Workflows", "Editorial workflows in Drupal manage content through draft, review, and published states. The Workbench Moderation module provides configurable states and transitions. Role-based permissions control who can create, edit, and publish content at each stage."),
  array("Redis Object Caching for Improved Performance", "Redis provides in-memory object caching that reduces database queries and improves page load times. Pantheon offers Redis as a platform service with automatic failover. The Redis module integrates Drupal cache bins with the Redis backend for persistent object storage."),
);
foreach ($articles as $a) {
  $node = new stdClass();
  $node->type = "article";
  $node->title = $a[0];
  $node->language = LANGUAGE_NONE;
  $node->body[LANGUAGE_NONE][0] = array("value" => $a[1], "format" => "filtered_html");
  $node->status = 1;
  $node->promote = 1;
  node_save($node);
  echo "Created: " . $node->title . "\n";
}
'

    # -----------------------------------------------------------------------
    # Step 4: Verify setup
    # -----------------------------------------------------------------------
    echo "[5/5] Verifying setup..."

    terminus drush "$SITE_ENV" -- ev '
$count = db_query("SELECT COUNT(*) FROM {node}")->fetchField();
if ($count < 20) {
  echo "FAIL: Expected 20 nodes, found " . $count . "\n";
  exit(1);
}
echo "Node count: " . $count . " (OK)\n";

$solr_check = db_query("SELECT COUNT(*) FROM {node} WHERE title LIKE :pattern", array(":pattern" => "%Solr%"))->fetchField();
echo "Nodes containing Solr: " . $solr_check . "\n";

$terminus_check = db_query("SELECT COUNT(*) FROM {node} WHERE title LIKE :pattern", array(":pattern" => "%Terminus%"))->fetchField();
echo "Nodes containing Terminus: " . $terminus_check . "\n";
'

    echo ""
    echo "=== Bootstrap complete ==="
    echo "Site: ${SITE_NAME_LC}"
    echo "Site ID: ${SITE_ID}"
    echo "Dev URL: https://dev-${SITE_NAME_LC}.pantheonsite.io"
    echo "Admin: admin / admin"
    echo ""
    echo "Next steps:"
    echo "  1. Add TERMINUS_TOKEN, PANTHEON_SSH_KEY secrets to the drops-7 repo"
    echo "  2. Add PANTHEON_SITE_D7 repo variable = ${SITE_NAME_LC}"
    echo ""
    echo "CI multidevs will inherit article content from dev."
    echo "Modules (apachesolr, search_api_solr) are installed per-multidev by solr9-setup-multidev.sh."
}

main "$@"
