<template>
	<Teleport to="body">
		<div v-if="modelValue" class="modal-backdrop" @click.self="close">
			<div class="modal-window">
				<header class="modal-header">
					<slot name="header">ℹ️</slot>
					<button class="modal-close" @click="close">×</button>
				</header>
				<section class="modal-body">
					<slot />
				</section>
				<footer class="modal-footer">
					<slot name="footer">
						<button class="btn" @click="close">{{ $t('buttons.cancel') }}</button>
						<button class="btn btn-primary" @click="confirm">{{ $t('buttons.ok') }}</button>
					</slot>
				</footer>
			</div>
		</div>
	</Teleport>
</template>

<script lang="ts">
export { default } from './modal'
</script>

<style src="./modal.scss"></style>

<style lang="scss" scoped></style>
